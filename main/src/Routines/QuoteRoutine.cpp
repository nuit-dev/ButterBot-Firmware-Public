#include "QuoteRoutine.h"
#include <esp_log.h>
#include <Phrases.h>
#include <QuoteText.h>
#include <Statics/ApplicationStatics.h>
#include <Core/Application.h>
#include <Services/Audio/Audio.h>
#include <Util/ServiceLocator.h>
#include "Audio/SpeechAudioGen.h"
#include "Audio/SpeechAudioSource.h"
#include "Audio/SpeechGen.h"
#include "Audio/VoicePreset.h"
#include "Services/Com.h"
#include "Services/ShutdownService.h"
#include "States/BBStateMachine.h"

static constexpr const char* TAG = "QuoteRoutine";

QuoteRoutine::QuoteRoutine(BBStateMachine* sm, QuoteData::Category category, BB::Action::Scenario scenario) :
		Routine(sm), category(category), scenario(scenario){}

QuoteRoutine::~QuoteRoutine(){
	if(voiceOverridden){
		Voice::clearOverride();
	}

	// Routine ended early (Shut Up, new scenario...) - don't leave speech running
	if(aborted){
		if(Audio* audio = ApplicationStatics::getApplication()->getService<Audio>()){
			audio->stop();
		}
	}
}

Routine::TickingState QuoteRoutine::tick(float deltaTime){
	const Application* app = ApplicationStatics::getApplication();
	Audio* audio = app->getService<Audio>();
	Com* com = app->getService<Com>();

	if(audio == nullptr || com == nullptr || !ServiceLocator::SpeechAudioGenInstance || !ServiceLocator::SpeechGenInstance){
		ESP_LOGE(TAG, "Missing required service(s)");
		return TickingState::Done;
	}

	if(!started){
		started = true;

		const Phrase phrase = QuoteText::phraseFor(category);
		id = Phrases::get(phrase);
		if(id < 0){
			return TickingState::Done;
		}

		const std::string text = Phrases::map(phrase, id);
		split = text.size() > QuoteText::SplitThreshold;
		parts = split ? QuoteText::splitSentences(text) : std::vector<std::string>{ text };

		switch(category){
			case QuoteData::Category::Darth: Voice::setOverride(VoicePreset::Vader); voiceOverridden = true; break;
			case QuoteData::Category::Hawking: Voice::setOverride(VoicePreset::Hawking); voiceOverridden = true; break;
			case QuoteData::Category::Hal:
			case QuoteData::Category::Daisy: Voice::setOverride(VoicePreset::Hal); voiceOverridden = true; break;
			case QuoteData::Category::Toaster: Voice::setOverride(VoicePreset::Toaster); voiceOverridden = true; break;
			case QuoteData::Category::Yoda: Voice::setOverride(VoicePreset::Yoda); voiceOverridden = true; break;
			default: break;
		}

		sm->bindRoutine(com->OnCommand, this, &QuoteRoutine::onCommand);
		return playPart(0) ? TickingState::Continue : TickingState::Done;
	}

	if(aborted){
		audio->stop();
		return TickingState::Done;
	}

	if(audio->isPlaying()){
		return TickingState::Continue;
	}

	if(nextPart >= parts.size()){
		// SHUTDOWN menu item: HAL has finished singing, the robot powers off (Shut Up / Poke above cancels it)
		if(category == QuoteData::Category::Daisy){
			ShutdownService::Shutdown(ShutdownReason::Command, false);
		}
		return TickingState::Done;
	}

	return playPart(nextPart) ? TickingState::Continue : TickingState::Done;
}

bool QuoteRoutine::playPart(size_t index){
	const Application* app = ApplicationStatics::getApplication();
	Audio* audio = app->getService<Audio>();
	Com* com = app->getService<Com>();
	if(index >= parts.size()){
		return false;
	}

	// DAISY: every line a bit slower and lower, the last one fully "dying"
	if(category == QuoteData::Category::Daisy){
		Voice::dying = parts.size() > 1 ? (float)index / (float)(parts.size() - 1) : 0.0f;
	}

	com->sendData(BB::State::Scenario, scenario, QuoteData{
		.category = category,
		.id = static_cast<uint8_t>(id),
		.part = split ? static_cast<uint8_t>(index) : QuoteData::WholeQuote
	});

	audio->play(ServiceLocator::SpeechAudioGenInstance.get(), std::make_unique<SpeechAudioSource>(SpeechGen::InputType::Text, parts[index]));
	nextPart = index + 1;
	return true;
}

void QuoteRoutine::onCommand(Ctrl::Command cmd){
	// Shut Up (mute) or Poke stops a quote that is still going
	if(cmd == Ctrl::Command::ShutUp || cmd == Ctrl::Command::Poke){
		aborted = true;
	}
}
