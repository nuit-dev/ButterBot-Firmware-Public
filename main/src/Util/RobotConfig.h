#ifndef BUTTERBOT_FIRMWARE_ROBOTCONFIG_H
#define BUTTERBOT_FIRMWARE_ROBOTCONFIG_H

#include <atomic>
#include <cstdint>

// Custom (NUIT): volume and night mode from the controller's Settings screen (Ctrl::RobotConfig), also kept in NVS
namespace RobotConfig {
	inline std::atomic<uint8_t> volume{ 80 };      // % - stock started at 0.8 gain
	inline std::atomic<uint8_t> nightMode{ 0 };    // NightMode
	inline std::atomic<uint8_t> nightVolume{ 40 }; // %
	// True while the clock is inside the night range - no idle comments, wandering or breathing, night volume
	inline std::atomic<bool> night{ false };
}

#endif //BUTTERBOT_FIRMWARE_ROBOTCONFIG_H
