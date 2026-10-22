#ifndef BUTTERBOT_FIRMWARE_TOASTERFIDGETROUTINE_H
#define BUTTERBOT_FIRMWARE_TOASTERFIDGETROUTINE_H

#include <cstdint>
#include "Routine.h"
#include "Services/BaseBoard.h"

// Custom (NUIT): with the TALKIE TOASTER voice the idle "wander" is a few small twitches on the spot instead of
// driving around: a short turn one way, back the other way, and settle - no net change of position or heading.
class ToasterFidgetRoutine : public Routine {
public:
	using Routine::Routine;

	virtual ~ToasterFidgetRoutine() override;
	TickingState tick(float deltaTime) override;

private:
	struct Step {
		int8_t left;
		int8_t right;
		uint32_t durationMs;
	};

	// Rotation timing reference (WanderRoutine / VoiceTurn180Routine): 180 degrees at +-100 takes 4000 ms
	static constexpr uint32_t MsPerDegree = 4000 / 180;
	static constexpr int8_t MotorSpeed = 100;
	// A twitch has to be this big to be seen: the motors need a moment to get going, so a few degrees hardly move it
	static constexpr uint16_t MinTwitchDeg = 25;
	static constexpr uint16_t MaxTwitchDeg = 40;
	static constexpr uint8_t MaxSteps = 7;

	Step steps[MaxSteps] = {};
	uint8_t stepCount = 0;
	uint8_t stepIndex = 0;
	uint64_t stepEndMs = 0;
	bool started = false;

	BaseBoard* baseBoard = nullptr;

	void build();
	void startStep();
};

#endif //BUTTERBOT_FIRMWARE_TOASTERFIDGETROUTINE_H
