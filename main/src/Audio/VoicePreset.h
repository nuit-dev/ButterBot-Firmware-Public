#ifndef BUTTERBOT_FIRMWARE_VOICEPRESET_H
#define BUTTERBOT_FIRMWARE_VOICEPRESET_H

#include <atomic>
#include <cstdint>

// Custom (NUIT): TTS voice preset
enum class VoicePreset : uint8_t {
	Normal, Hawking, Vader, Hal, Toaster, Yoda
};

namespace Voice {
	// Chosen on the controller's Settings screen
	inline std::atomic<VoicePreset> user{ VoicePreset::Normal };

	// Set by a routine that must always use one voice (DARTH OVERKLOKING, OVERHAWKING, HAL 9000, DAISY)
	inline std::atomic<bool> overridden{ false };
	inline std::atomic<VoicePreset> override{ VoicePreset::Normal };

	inline void setOverride(VoicePreset preset){
		override = preset;
		overridden = true;
	}

	// DAISY only: 0 = normal HAL, 1 = HAL at the end of the song (much slower and lower, "running down")
	inline std::atomic<float> dying{ 0.0f };

	inline void clearOverride(){
		overridden = false;
		dying = 0.0f;
	}

	// Read at the start of every utterance by SpeechGen and SpeechAudioGen
	inline VoicePreset current(){
		return overridden ? override.load() : user.load();
	}
}

#endif //BUTTERBOT_FIRMWARE_VOICEPRESET_H
