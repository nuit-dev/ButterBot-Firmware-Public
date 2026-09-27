#ifndef BUTTERBOT_FIRMWARE_SPEECHAUDIOGEN_H
#define BUTTERBOT_FIRMWARE_SPEECHAUDIOGEN_H

#include "Services/Audio/AudioGenerator.h"
#include "Reverb.h"
#include "Compressor.h"
#include "VoicePreset.h"
/**
 * Interface between CMF Audio service and locally defined SpeechGen service.
 *
 * Basically just a passthrough of data from AudioSource, which is expected to be a SpeechAudioSource.
 */
class SpeechAudioGen : public AudioGenerator {
	using SampleType = int16_t;

public:
	SpeechAudioGen();

	void open(std::unique_ptr<AudioSource> resource) override;

	void close() override;

	size_t getData(uint8_t* buffer, size_t bytes) override;

private:
	std::unique_ptr<AudioSource> resource;
	int bytesRemaining = 0;
	Compressor compressor;
	Reverb reverb;

	// Custom (NUIT): per-preset effects (Voice::current()), latched in open()
	struct FxParams {
		float slowdown;  // resample factor, >1 = lower pitch AND formants, slower (bigger "body")
		float hpf;       // high-pass cutoff Hz, 0 = off
		float lpf;       // low-pass cutoff Hz, 0 = off
		float peakFreq;  // peaking EQ Hz, 0 = off
		float peakA;     // peaking EQ amplitude, 10^(dB/40): 1.19 = +3 dB, 1.33 = +5 dB
		float drive;     // soft-clip amount, 0 = clean
		float comb;      // mask resonance feedback, 0 = off
		uint8_t combDelay; // samples, 36 = ~2.3 ms = hollow ~440 Hz helmet
		bool breath;     // Vader: mask breathing after every utterance
		float reverbWet;
	};
	static constexpr FxParams NormalFx = { 1.0f, 0, 0, 0, 0, 0, 0, 0, false, 0.05f };
	static constexpr FxParams HawkingFx = { 1.0f, 200, 5000, 2500, 1.334f, 0, 0, 0, false, 0.03f }; // bright, "tinny" formant-synth colour
	static constexpr FxParams VaderFx = { 1.25f, 0, 2000, 0, 0, 2.0f, 0.4f, 36, true, 0.15f };
	static constexpr FxParams HalFx = { 1.1f, 80, 3400, 250, 1.189f, 0, 0, 0, false, 0.05f };      // warm, close mic
	static const FxParams& fxFor(VoicePreset preset);

	// Voice::dying (DAISY): extra resample slowdown and a duller low-pass at the end of the song
	static constexpr float DyingSlowdown = 0.5f;
	static constexpr float DyingLpf = 0.4f;

	const FxParams* fx = &NormalFx;
	float slowdown = 1.0f;

	struct Biquad {
		enum class Type : uint8_t { LowPass, HighPass, Peak };
		float b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0;
		float x1 = 0, x2 = 0, y1 = 0, y2 = 0;
		void setup(Type type, float freq, float peakA = 1);
		void reset(){ x1 = x2 = y1 = y2 = 0; }
		float process(float x){
			const float y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
			x2 = x1; x1 = x;
			y2 = y1; y1 = y;
			return y;
		}
	};
	Biquad hpf, lpf, peak;

	static constexpr size_t CombMax = 64;
	float combBuf[CombMax];
	uint8_t combPos = 0;

	void processFx(int16_t* samples, size_t count);

	// Linear-interpolation resampler state
	static constexpr size_t SrcBufSize = 256;
	int16_t srcBuf[SrcBufSize];
	size_t srcLen = 0, srcPos = 0;
	float resPhase = 1.0f;
	int16_t resPrev = 0, resNext = 0;
	bool srcEnded = false;
	bool nextSrcSample(int16_t& out);
	size_t readSource(uint8_t* buffer, size_t bytes);
	size_t readResampled(int16_t* out, size_t count);

	// Vader breathing: filtered noise, inhale - pause - exhale, played after the speech ends
	static constexpr uint32_t BreathIn = 16800;   // 1.05 s
	static constexpr uint32_t BreathGap = 4000;   // 0.25 s
	static constexpr uint32_t BreathOut = 20800;  // 1.3 s
	static constexpr float BreathInLevel = 0.18f;
	static constexpr float BreathOutLevel = 0.13f;
	static constexpr float BreathVariation = 0.15f; // every breath +-15 % length and level
	static constexpr float HissLevel = 0.012f;      // respirator hiss under the speech
	uint32_t breathIn = BreathIn, breathOut = BreathOut;
	float breathInLevel = BreathInLevel, breathOutLevel = BreathOutLevel;
	uint32_t breathPos = 0;
	float noise(); // white noise [-1, 1]
	float random01();
	Biquad hissHpf;
	uint32_t noiseState = 0x12345678;
	Biquad breathHpf, breathLpf;
	size_t fillBreath(int16_t* out, size_t count);
};


/**
 * Custom (NUIT): empty source. Played through SpeechAudioGen with the VADER voice it produces one mask breath
 * (IdleState breathes like this every 10-20 s while nothing else is going on).
 */
class BreathOnlySource : public AudioSource {
public:
	void open() override{}
	void close() override{}
	size_t getData(uint8_t* buffer, size_t bytes) override{ return 0; }
	operator bool() const override{ return false; }
};

#endif //BUTTERBOT_FIRMWARE_SPEECHAUDIOGEN_H
