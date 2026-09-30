#include "SpeechAudioGen.h"
#include "Core/Application.h"
#include <cmath>

DEFINE_LOG(SpeechAudioGen)

SpeechAudioGen::SpeechAudioGen(){
	reverb.setRoomSize(0.2f); // 0 = metalan/sjajan, 1 = taman/prigusen
	reverb.setDamping(0.8f); // duzina odjeka
	reverb.setWet(NormalFx.reverbWet); // Koliko reverba cujes
}

const SpeechAudioGen::FxParams& SpeechAudioGen::fxFor(VoicePreset preset){
	switch(preset){
		case VoicePreset::Hawking: return HawkingFx;
		case VoicePreset::Vader: return VaderFx;
		case VoicePreset::Hal: return HalFx;
		case VoicePreset::Toaster: return ToasterFx;
		case VoicePreset::Yoda: return YodaFx;
		default: return NormalFx;
	}
}

void SpeechAudioGen::Biquad::setup(Type type, float freq, float peakA){
	// RBJ cookbook, Q = 0.707 (peak: Q = 1), 16 kHz
	const float w0 = 2.0f * (float)M_PI * freq / 16000.0f;
	const float cosw0 = cosf(w0);
	const float alpha = sinf(w0) / (2.0f * (type == Type::Peak ? 1.0f : 0.707f));
	float a0;
	switch(type){
		case Type::HighPass:
			b0 = (1.0f + cosw0) / 2.0f;
			b1 = -(1.0f + cosw0);
			b2 = b0;
			a0 = 1.0f + alpha;
			a2 = 1.0f - alpha;
			break;
		case Type::Peak: {
			const float A = peakA;
			b0 = 1.0f + alpha * A;
			b1 = -2.0f * cosw0;
			b2 = 1.0f - alpha * A;
			a0 = 1.0f + alpha / A;
			a2 = 1.0f - alpha / A;
			break;
		}
		default:
			b0 = (1.0f - cosw0) / 2.0f;
			b1 = 1.0f - cosw0;
			b2 = b0;
			a0 = 1.0f + alpha;
			a2 = 1.0f - alpha;
			break;
	}
	a1 = -2.0f * cosw0 / a0;
	a2 /= a0;
	b0 /= a0;
	b1 /= a0;
	b2 /= a0;
	reset();
}

void SpeechAudioGen::open(std::unique_ptr<AudioSource> resource){
	resource->open();
	this->resource = std::move(resource);
	compressor.reset();

	const float d = Voice::dying.load();
	fx = &fxFor(Voice::current());
	slowdown = fx->slowdown * (1.0f + DyingSlowdown * d);
	reverb.setWet(fx->reverbWet);
	if(fx->hpf > 0) hpf.setup(Biquad::Type::HighPass, fx->hpf);
	if(fx->lpf > 0) lpf.setup(Biquad::Type::LowPass, fx->lpf * (1.0f - DyingLpf * d));
	if(fx->peakFreq > 0) peak.setup(Biquad::Type::Peak, fx->peakFreq, fx->peakA);
	for(float& s : combBuf) s = 0;
	combPos = 0;

	srcLen = srcPos = 0;
	resPhase = 1.0f;
	resPrev = resNext = 0;
	srcEnded = false;

	breathPos = 0;
	breathIn = (uint32_t)(BreathIn * (1.0f + BreathVariation * (2.0f * random01() - 1.0f)));
	breathOut = (uint32_t)(BreathOut * (1.0f + BreathVariation * (2.0f * random01() - 1.0f)));
	breathInLevel = BreathInLevel * (1.0f + BreathVariation * (2.0f * random01() - 1.0f));
	breathOutLevel = BreathOutLevel * (1.0f + BreathVariation * (2.0f * random01() - 1.0f));
	hissHpf.setup(Biquad::Type::HighPass, 2500);
	breathHpf.setup(Biquad::Type::HighPass, 300);
	breathLpf.setup(Biquad::Type::LowPass, 1800);
}

void SpeechAudioGen::close(){
	resource->close();
	resource.reset();
}

size_t SpeechAudioGen::readSource(uint8_t* buffer, size_t bytes){
	size_t bytesTransferred = 0;
	while(bytesTransferred < bytes && (bool)*resource){
		auto i = resource->getData(buffer + bytesTransferred, bytes - bytesTransferred);
		if(i == 0) break;
		bytesTransferred += i;
	}
	return bytesTransferred;
}

bool SpeechAudioGen::nextSrcSample(int16_t& out){
	if(srcPos >= srcLen){
		if(srcEnded) return false;
		srcLen = readSource((uint8_t*)srcBuf, sizeof(srcBuf)) / sizeof(int16_t);
		srcPos = 0;
		if(srcLen == 0){
			srcEnded = true;
			return false;
		}
	}
	out = srcBuf[srcPos++];
	return true;
}

size_t SpeechAudioGen::readResampled(int16_t* out, size_t count){
	const float step = 1.0f / slowdown;
	size_t produced = 0;
	while(produced < count){
		while(resPhase >= 1.0f){
			int16_t s;
			if(!nextSrcSample(s)) return produced;
			resPrev = resNext;
			resNext = s;
			resPhase -= 1.0f;
		}
		out[produced++] = (int16_t)(resPrev + (resNext - resPrev) * resPhase);
		resPhase += step;
	}
	return produced;
}

float SpeechAudioGen::noise(){
	// xorshift32
	noiseState ^= noiseState << 13;
	noiseState ^= noiseState >> 17;
	noiseState ^= noiseState << 5;
	return (int32_t)noiseState * (1.0f / 2147483648.0f);
}

float SpeechAudioGen::random01(){
	return noise() * 0.5f + 0.5f;
}

size_t SpeechAudioGen::fillBreath(int16_t* out, size_t count){
	const uint32_t total = breathIn + BreathGap + breathOut;
	size_t produced = 0;
	while(produced < count && breathPos < total){
		float env = 0;
		if(breathPos < breathIn){
			env = breathInLevel * sinf((float)M_PI * breathPos / breathIn);
		}else if(breathPos >= breathIn + BreathGap){
			env = breathOutLevel * sinf((float)M_PI * (breathPos - breathIn - BreathGap) / breathOut);
		}
		const float x = breathLpf.process(breathHpf.process(noise())) * env;
		out[produced++] = (int16_t)(x * 32767.0f);
		breathPos++;
	}
	return produced;
}

void SpeechAudioGen::processFx(int16_t* samples, size_t count){
	for(size_t i = 0; i < count; i++){
		float x = samples[i] / 32768.0f;
		if(fx->hpf > 0) x = hpf.process(x);
		if(fx->lpf > 0) x = lpf.process(x);
		if(fx->peakFreq > 0) x = peak.process(x);
		if(fx->drive > 0){
			// Soft clip, keeps +-1 at +-1
			x = x * (1.0f + fx->drive) / (1.0f + fx->drive * fabsf(x));
		}
		if(fx->comb > 0){
			// Feedback comb = hollow mask cavity
			const float y = x + fx->comb * combBuf[combPos];
			combBuf[combPos] = y;
			combPos = (combPos + 1) % fx->combDelay;
			x = y * (1.0f - fx->comb);
		}
		x *= fx->gain;
		if(x > 1.0f) x = 1.0f;
		else if(x < -1.0f) x = -1.0f;
		samples[i] = (int16_t)(x * 32767.0f);
	}
}

size_t SpeechAudioGen::getData(uint8_t* buffer, size_t bytes){
	if(resource == nullptr){
		CMF_LOG(SpeechAudioGen, LogLevel::Debug, "early return, no resource");
		return 0;
	}

	auto samples = reinterpret_cast<int16_t*>(buffer);
	const size_t maxSamples = bytes / sizeof(int16_t);
	size_t sampleCount;
	if(slowdown != 1.0f){
		sampleCount = readResampled(samples, maxSamples);
	}else{
		sampleCount = readSource(buffer, bytes) / sizeof(int16_t);
	}

	compressor.process(samples, sampleCount);

	if(fx->breath){
		// Respirator hiss under the speech, goes through the mask below
		for(size_t i = 0; i < sampleCount; i++){
			const int32_t s = samples[i] + (int32_t)(hissHpf.process(noise()) * HissLevel * 32767.0f);
			samples[i] = (int16_t)(s > 32767 ? 32767 : s < -32768 ? -32768 : s);
		}
	}

	// Speech is over (short read) - Vader breathes in the mask before the next line
	if(fx->breath && sampleCount < maxSamples && !(bool)*resource){
		sampleCount += fillBreath(samples + sampleCount, maxSamples - sampleCount);
	}

	if(fx != &NormalFx){
		processFx(samples, sampleCount);
	}
	reverb.process(samples, sampleCount);
	return sampleCount * sizeof(int16_t);
}
