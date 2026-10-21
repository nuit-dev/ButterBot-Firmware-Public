#ifndef BUTTERBOT_FIRMWARE_ROBOTCONFIG_H
#define BUTTERBOT_FIRMWARE_ROBOTCONFIG_H

#include <atomic>
#include <cstdint>

// Custom (NUIT): volume, night mode and chatter frequency from the controller's Settings screen (Ctrl::RobotConfig), also kept in NVS
namespace RobotConfig {
	inline std::atomic<uint8_t> volume{ 80 };      // % - stock started at 0.8 gain
	inline std::atomic<uint8_t> nightMode{ 0 };    // NightMode
	inline std::atomic<uint8_t> nightVolume{ 40 }; // %
	// False = ROAMING OFF: no idle wandering, PersonRoutine looks straight ahead only (commands still move it)
	inline std::atomic<bool> roaming{ true };
	// True while the clock is inside the night range - no idle comments, wandering or breathing, night volume
	inline std::atomic<bool> night{ false };
	// Chatter frequency: ChatterMode (0=Off, 1=Rare, 2=Normal, 3=Frequent, 4=Toaster)
	inline std::atomic<uint8_t> chatter{ 2 };
}

#endif //BUTTERBOT_FIRMWARE_ROBOTCONFIG_H
