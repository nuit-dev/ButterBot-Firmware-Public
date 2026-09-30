#ifndef BUTTERBOT_FIRMWARE_SPEECHGEN_H
#define BUTTERBOT_FIRMWARE_SPEECHGEN_H

#include <memory>
#include <span>
#include "flite.h"
#include "freertos/ringbuf.h"
#include "Thread/Threaded.h"
#include "VoicePreset.h"

/**
 * Flite thread, koji vrti TTS
 * callback puni ringbuffer/queue
 * generate vuče iz buffera/queuea, šalje dalje, odblokirava flite thread (ako je napunjen buff)
 *
 * ako je currently running TTS thread, svaki novi parse poziv prazni buffer i vraća CST_AUDIO_STREAM_STOP
 *
 *
 */
class SpeechGen {
public:
	SpeechGen();
	virtual ~SpeechGen();
	/**
	 * Retrieves speech synthesis audio.
	 * @param output std::span to be filled with speech synthesis data
	 * @param last output parameter, true = last output of current synthesis reached, false = current synthesis still ongoing
	 * @param timeout maximum time to wait for data, 0 for non-blocking call
	 * @return size of retrieved data, in samples; zero if synthesis pending or not started
	 */
	int generate(std::span<int16_t> output, bool& last, TickType_t timeout = 0);

	/**
	 * Initializes speech synthesis with given text.
	 * Aborts current synthesis and empties the buffer if synthesis is currently running.
	 * @param text US English text to be synthesized
	 */
	void parseText(std::string text);

	/**
	 * Initializes speech synthesis with given phonemes.
	 * Aborts current synthesis and empties the buffer if synthesis is currently running.
	 * @param phonemes Phonemes written in ESP-SR's Multinet format (best obtained directly from AudioFrontend's speech mode)
	 */
	void parsePhonemes(const std::string& phonemes);

	/**
	 * Initializes speech synthesis with given SSML string (https://en.wikipedia.org/wiki/Speech_Synthesis_Markup_Language)
	 * Aborts current synthesis and empties the buffer if synthesis is currently running.
	 * @param ssml SSML string
	 */
	void parseSSML(std::string ssml);

	/**
	 * Cancels ongoing synthesis and purges buffers.
	 */
	void cancel();

	enum class InputType : uint8_t {
		Text, Phonemes, SSML
	};

private:
	static constexpr size_t BufferSize = 4 * 1024;
	static constexpr size_t FliteMinBufSize = 512;

	std::unique_ptr<Threaded> fliteThread;

	void fliteThreadFunc();

	int fliteCallback(const cst_wave* w, int start, int size, int last, cst_audio_streaming_info* asi);

	SemaphoreHandle_t startSem;
	SemaphoreHandle_t endSem;
	cst_audio_streaming_info* asi;
	cst_voice* voice;

	RingbufHandle_t ringbuf;

	std::string fliteInput;

	InputType fliteInputType = InputType::Text;

	//Aborts flite's synthesis stream (flite_XXX_to_speech function calls)
	std::atomic_bool abort = false;
	std::atomic_bool synthRunning = false;

	void checkAndAbort();

	// Custom (NUIT): voice presets (Voice::current()), flite features of cmu_us_kal16 (stock: mean 95 Hz, stddev 11, stretch 1.1).
	// SpeechAudioGen may slow the audio down afterwards (Vader, HAL), which lowers pitch and speed once more.
	struct PresetParams {
		float f0Mean;    // int_f0_target_mean, Hz
		float f0Stddev;  // int_f0_target_stddev, 0 = monotone
		float stretch;   // duration_stretch, >1 = slower
	};
	static constexpr PresetParams NormalParams = { 95.0f, 11.0f, 1.1f };
	static constexpr PresetParams HawkingParams = { 120.0f, 10.0f, 1.2f }; // DECtalk "Perfect Paul": ~120 Hz, normal intonation range; ~150 wpm like Hawking's delivery
	static constexpr PresetParams VaderParams = { 72.0f, 4.0f, 1.14f }; // x1.25 resample: ~58 Hz, stretch ~1.43
	static constexpr PresetParams HalParams = { 96.0f, 6.0f, 1.25f };   // x1.1 resample: ~87 Hz, stretch ~1.38, calm, soft intonation
	static constexpr PresetParams ToasterParams = { 170.0f, 22.0f, 0.95f }; // x0.9 resample: ~190 Hz, fast (~0.86), chirpy
	static constexpr PresetParams YodaParams = { 140.0f, 24.0f, 1.35f };    // x0.8 resample: ~165 Hz, small head, slowish (~1.08), lilting

	// Voice::dying (DAISY): at 1 pitch is DyingPitch x lower, stretch DyingStretch x longer, intonation gone
	static constexpr float DyingPitch = 0.3f;
	static constexpr float DyingStretch = 0.6f;

	VoicePreset appliedPreset = VoicePreset::Normal; // flite thread only
	float appliedDying = 0.0f;
	void applyPreset();

	[[maybe_unused]] static std::string flitePhoneToCMU(const cst_item* seg);
	static std::string CMUToFlitePhone(const std::string& cmu);
	static std::string multinetToCMU(const std::string& input);
};


#endif //BUTTERBOT_FIRMWARE_SPEECHGEN_H
