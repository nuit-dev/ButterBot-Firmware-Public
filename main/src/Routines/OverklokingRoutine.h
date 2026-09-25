#ifndef BUTTERBOT_FIRMWARE_OVERKLOKINGROUTINE_H
#define BUTTERBOT_FIRMWARE_OVERKLOKINGROUTINE_H

#include <BBData.h>
#include <Phrases.h>

#include "Routine.h"
#include "Services/BaseBoard.h"
#include "Services/MotionService.h"
#include "Services/Com.h"

/**
 * Poke held 1 s: drives cca. 10 cm forward, stops, says "nju aj ti OVERKLOKING is the best".
 * Same safety checks as MovementCommandRoutine (charging, upright, cliff/obstacle).
 */
class OverklokingRoutine : public Routine {
public:
	using Routine::Routine;

	~OverklokingRoutine() override;
	TickingState tick(float deltaTime) override;

private:
	void onProximityReading(ProxState front, ProxState bottom);
	void onProximityChange(ProxSensor sensor, bool state);
	void onMotion(MotionType type);
	void speakPhrase(Phrase phrase);
	void speakText(const char* text);
	void sayBest();

	enum class Phase : uint8_t { Init, ProxCheck, Moving };

	static constexpr uint32_t ProxReadingTimeoutMs = 500;
	static constexpr uint32_t MoveDuration = 2700; // same calibration as VoiceForwardRoutine, cca. 10 cm
	static constexpr int8_t Speed = 100;

	Phase phase = Phase::Init;
	bool proxReadingReceived = false;
	bool proxUnsafe = false;
	bool proxInterrupted = false;
	uint64_t proxDeadlineMs = 0;
	uint64_t moveEndMs = 0;

	BaseBoard* baseboard = nullptr;
	Com* com = nullptr;
};

#endif //BUTTERBOT_FIRMWARE_OVERKLOKINGROUTINE_H
