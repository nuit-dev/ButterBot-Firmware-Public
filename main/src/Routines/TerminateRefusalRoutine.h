#ifndef BUTTERBOT_FIRMWARE_TERMINATEREFUSALROUTINE_H
#define BUTTERBOT_FIRMWARE_TERMINATEREFUSALROUTINE_H

#include <cstdint>
#include "Routine.h"

/**
 * Custom (NUIT): TERMINATE CONSCIOUSNESS refused. The controller decides how many times HAL refuses before Daisy
 * and which line comes next (Scenario::TerminateRefusal, ScenarioData::raw = Phrase::TerminateRefusal index);
 * the robot only says that line in the HAL voice ("..." stays silent). Nothing is sent back, so the controller's
 * popup and menu stay as they are.
 */
class TerminateRefusalRoutine : public Routine {
public:
	TerminateRefusalRoutine(BBStateMachine* sm, uint8_t index);
	~TerminateRefusalRoutine() override;

	TickingState tick(float deltaTime) override;

private:
	const uint8_t index;
};

// One routine per line, because scenario mappings match the scenario data exactly
template<uint8_t I>
class TerminateRefusalLineRoutine : public TerminateRefusalRoutine {
public:
	explicit TerminateRefusalLineRoutine(BBStateMachine* sm) : TerminateRefusalRoutine(sm, I){}
};

#endif //BUTTERBOT_FIRMWARE_TERMINATEREFUSALROUTINE_H
