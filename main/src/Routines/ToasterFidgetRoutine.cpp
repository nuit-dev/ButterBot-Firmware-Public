#include "ToasterFidgetRoutine.h"
#include <cstdlib>
#include <Util/stdafx.h>
#include <Core/Application.h>
#include <Statics/ApplicationStatics.h>
#include "Services/MotionService.h"

DEFINE_LOG(ToasterFidgetRoutine)

ToasterFidgetRoutine::~ToasterFidgetRoutine(){
	if(baseBoard){
		baseBoard->stopMotors();
	}
}

void ToasterFidgetRoutine::build(){
	// 2 twitches (a right, a left) or 3 (a right, a + c left, c right), a and c 8-18 degrees, the first direction
	// random. Right and left always add up to the same angle, so it ends facing where it started.
	const bool startRight = rand() % 2;
	const uint16_t a = 8 + rand() % 11;
	const uint16_t c = 8 + rand() % 11;
	const bool three = rand() % 2;

	const uint16_t degrees[3] = { a, three ? static_cast<uint16_t>(a + c) : a, c };
	const uint8_t count = three ? 3 : 2;

	for(uint8_t i = 0; i < count; i++){
		const bool right = (i % 2 == 0) == startRight;
		steps[stepCount++] = {
			static_cast<int8_t>(right ? MotorSpeed : -MotorSpeed),
			static_cast<int8_t>(right ? -MotorSpeed : MotorSpeed),
			degrees[i] * MsPerDegree
		};
		if(i != count - 1){
			steps[stepCount++] = { 0, 0, 120u + static_cast<uint32_t>(rand() % 160) };
		}
	}
}

void ToasterFidgetRoutine::startStep(){
	const Step& step = steps[stepIndex];
	if(step.left == 0 && step.right == 0){
		baseBoard->stopMotors();
	}else{
		baseBoard->setMotors(step.left, step.right);
	}
	stepEndMs = millis() + step.durationMs;
}

Routine::TickingState ToasterFidgetRoutine::tick(float deltaTime){
	if(!started){
		const Application* app = ApplicationStatics::getApplication();
		baseBoard = app->getService<BaseBoard>();
		MotionService* motionService = app->getService<MotionService>();

		if(baseBoard == nullptr || motionService == nullptr){
			CMF_LOG(ToasterFidgetRoutine, LogLevel::Error, "Missing required service(s)");
			return TickingState::Done;
		}

		// Not while docked / charging, and only when it stands on the ground
		if(baseBoard->getChargingState() != ChargingState::Unplugged || !motionService->isUprightOnGround()){
			return TickingState::Done;
		}

		build();
		started = true;
		stepIndex = 0;
		startStep();
		return TickingState::Continue;
	}

	if(millis() < stepEndMs){
		return TickingState::Continue;
	}

	stepIndex++;
	if(stepIndex >= stepCount){
		baseBoard->stopMotors();
		return TickingState::Done;
	}

	startStep();
	return TickingState::Continue;
}
