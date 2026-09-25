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
#include "Services/Com.h"
#include "States/BBStateMachine.h"

static constexpr const char* TAG = "QuoteRoutine";

QuoteRoutine::QuoteRoutine(BBStateMachine* sm, QuoteData::Category category, BB::Action::Scenario scenario) :
		Routine(sm), category(category), scenario(scenario){}

QuoteRoutine::~QuoteRoutine(){
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
