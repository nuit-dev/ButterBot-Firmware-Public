#ifndef BUTTERBOT_FIRMWARE_BREATHROUTINE_H
#define BUTTERBOT_FIRMWARE_BREATHROUTINE_H

#include <cstdlib>
#include <memory>
#include <Statics/ApplicationStatics.h>
#include <Core/Application.h>
#include <Services/Audio/Audio.h>
#include <Util/ServiceLocator.h>
#include "Routine.h"
#include "Audio/SpeechAudioGen.h"

/**
 * Custom (NUIT): idle "comment" for the VADER voice - one mask breath, sometimes two in a row.
 * Picked from IdleState::RandomRoutines like Ramble, only while VOICE is VADER.
 */
class BreathRoutine : public Routine {
public:
	using Routine::Routine;

	TickingState tick(float deltaTime) override{
		Audio* audio = ApplicationStatics::getApplication()->getService<Audio>();
		if(audio == nullptr || !ServiceLocator::SpeechAudioGenInstance){
			return TickingState::Done;
		}

		if(!started){
			started = true;
			breathsLeft = (rand() % DoubleBreathOneIn == 0) ? 2 : 1;
		}

		if(audio->isPlaying()){
			return TickingState::Continue;
		}

		if(breathsLeft == 0){
			return TickingState::Done;
		}

		breathsLeft--;
		audio->play(ServiceLocator::SpeechAudioGenInstance.get(), std::make_unique<BreathOnlySource>());
		return TickingState::Continue;
	}

private:
	static constexpr int DoubleBreathOneIn = 4; // ~every 4th time it breathes twice
	bool started = false;
	uint8_t breathsLeft = 0;
};

#endif //BUTTERBOT_FIRMWARE_BREATHROUTINE_H
