#include "OverklokingRoutine.h"
#include <string>
#include <esp_log.h>
#include <Statics/ApplicationStatics.h>
#include <Util/stdafx.h>
#include <Core/Application.h>
#include <Services/Audio/Audio.h>
#include <Util/ServiceLocator.h>
#include "Audio/SpeechAudioGen.h"
#include "Audio/SpeechAudioSource.h"
#include "Audio/SpeechGen.h"
#include "States/BBStateMachine.h"
#include <QuoteText.h>

OverklokingRoutine::~OverklokingRoutine(){
	if(baseboard != nullptr){
		baseboard->stopMotors();
	}
}

Routine::TickingState OverklokingRoutine::tick(float deltaTime){
	const Application* app = ApplicationStatics::getApplication();

	if(phase == Phase::Init){
		baseboard = app->getService<BaseBoard>();
		com = app->getService<Com>();
		MotionService* motion = app->getService<MotionService>();

		if(baseboard == nullptr || motion == nullptr || com == nullptr){
			ESP_LOGE("OverklokingRoutine", "Missing required service(s)");
			return TickingState::Done;
		}

		// Can't drive: still say the line, just don't move
		if(baseboard->getChargingState() != ChargingState::Unplugged){
			com->sendData(BB::State::Idle, BB::Action::Idle::CantMove, CantMoveData{ .reason = CantMoveData::Reason::Charging });
			sayBest();
			return TickingState::Done;
		}

		if(!motion->isUprightOnGround()){
			com->sendData(BB::State::Idle, BB::Action::Idle::CantMove, CantMoveData{ .reason = CantMoveData::Reason::Unstable });
			sayBest();
			return TickingState::Done;
		}

		sm->bindRoutine(baseboard->onProximityReading, this, &OverklokingRoutine::onProximityReading);
		sm->bindRoutine(baseboard->onProximityChange, this, &OverklokingRoutine::onProximityChange);
		sm->bindRoutine(motion->onMotion, this, &OverklokingRoutine::onMotion);
		baseboard->requestProximityState();
		proxDeadlineMs = millis() + ProxReadingTimeoutMs;

		phase = Phase::ProxCheck;
		return TickingState::Continue;
	}

	if(phase == Phase::ProxCheck){
		if(proxUnsafe || proxInterrupted){
			speakPhrase(Phrase::CannotMove);
			return TickingState::Done;
		}
		if(!proxReadingReceived && millis() < proxDeadlineMs){
			return TickingState::Continue;
		}

		// Controller shows the stock "Go forward" window while moving
		VoiceControlData data{};
		data.direction = VoiceControlData::Direction::Forward;
		com->sendData(BB::State::Scenario, BB::Action::Scenario::VoiceControl, data);

		baseboard->setMotors(Speed, Speed);
		moveEndMs = millis() + MoveDuration;
		phase = Phase::Moving;
		return TickingState::Continue;
	}

	if(phase == Phase::Moving){
		if(proxInterrupted){
			baseboard->stopMotors();
			speakPhrase(Phrase::RoutineInterrupted);
			return TickingState::Done;
		}

		if(millis() >= moveEndMs){
			baseboard->stopMotors();
			sayBest();
			return TickingState::Done;
		}

		return TickingState::Continue;
	}

	return TickingState::Done;
}

void OverklokingRoutine::sayBest(){
	const int16_t id = Phrases::get(Phrase::OverklokingBest);
	if(id < 0) return;

	if(com != nullptr){
		com->sendData(BB::State::Scenario, BB::Action::Scenario::OverklokingDrive, QuoteData{
			.category = QuoteData::Category::OverklokingBest,
			.id = static_cast<uint8_t>(id),
			.part = QuoteData::WholeQuote
		});
	}
	speakText(Phrases::map(Phrase::OverklokingBest, id).c_str());
}

void OverklokingRoutine::speakText(const char* text){
	const Application* app = ApplicationStatics::getApplication();
	Audio* audio = app->getService<Audio>();

	if(audio == nullptr || !ServiceLocator::SpeechAudioGenInstance || !ServiceLocator::SpeechGenInstance){
		ESP_LOGE("OverklokingRoutine", "Missing required service(s)");
		return;
	}

	audio->play(ServiceLocator::SpeechAudioGenInstance.get(), std::make_unique<SpeechAudioSource>(SpeechGen::InputType::Text, std::string(text)));
	audio->waitEnd(portMAX_DELAY);
}

void OverklokingRoutine::speakPhrase(Phrase phrase){
	const int16_t id = Phrases::get(phrase);
	if(id < 0) return;
	speakText(Phrases::map(phrase, id).c_str());
}

void OverklokingRoutine::onProximityReading(ProxState front, ProxState bottom){
	proxReadingReceived = true;
	proxUnsafe = (bottom == ProxState::Uncovered) || (front == ProxState::Covered);
}

void OverklokingRoutine::onProximityChange(ProxSensor sensor, bool state){
	if((sensor == ProxSensor::Bottom && !state) || (sensor == ProxSensor::Front && state)){
		proxInterrupted = true;
	}
}

void OverklokingRoutine::onMotion(MotionType type){
	// Motion events only fire while lifted off the ground; treat any as an unsafe interruption
	proxInterrupted = true;
}
