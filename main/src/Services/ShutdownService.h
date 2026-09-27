#ifndef BUTTERBOT_FIRMWARE_SHUTDOWNSERVICE_H
#define BUTTERBOT_FIRMWARE_SHUTDOWNSERVICE_H

#include <BBData.h>

typedef ShutdownData::ShutdownReason ShutdownReason;

class ShutdownService {
public:
	// Custom (NUIT): speak = false skips the "turning off" line (SHUTDOWN menu item, after Daisy)
	static void Shutdown(ShutdownReason reason, bool speak = true);

	static void PowerOff();

private:
	static constexpr uint32_t SilentShutdownDelayMs = 3000; // controller's UninterruptedDisplayMs
};

#endif //BUTTERBOT_FIRMWARE_SHUTDOWNSERVICE_H
