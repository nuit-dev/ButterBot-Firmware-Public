#include "TerminateRefusalRoutine.h"
#include <algorithm>
#include <cctype>
#include <Statics/ApplicationStatics.h>
#include <Core/Application.h>
#include <Services/Audio/Audio.h>
#include <Phrases.h>
#include <Util/ServiceLocator.h>
#include "Audio/SpeechAudioGen.h"
#include "Audio/SpeechAudioSource.h"
#include "Audio/SpeechGen.h"
#include "Audio/VoicePreset.h"

DEFINE_LOG(TerminateRefusalRoutine)

TerminateRefusalRoutine::TerminateRefusalRoutine(BBStateMachine* sm, uint8_t index) : Routine(sm), index(index){}

TerminateRefusalRoutine::~TerminateRefusalRoutine(){
	Voice::clearOverride();
}

Routine::TickingState TerminateRefusalRoutine::tick(float deltaTime){
	Audio* audio = ApplicationStatics::getApplication()->getService<Audio>();
	if(audio == nullptr || !ServiceLocator::SpeechAudioGenInstance || !ServiceLocator::SpeechGenInstance){
		CMF_LOG(TerminateRefusalRoutine, LogLevel::Error, "Missing required service(s)");
		return TickingState::Done;
	}

	const std::string text = Phrases::map(Phrase::TerminateRefusal, index);
	// "..." - HAL just stares
	if(std::none_of(text.begin(), text.end(), [](unsigned char c){ return std::isalpha(c); })){
		return TickingState::Done;
	}

	// TALKIE TOASTER refuses in his own voice (the VOICE setting), everyone else as HAL
	if(!Phrases::toasterMode) Voice::setOverride(VoicePreset::Hal);
	audio->play(ServiceLocator::SpeechAudioGenInstance.get(), std::make_unique<SpeechAudioSource>(SpeechGen::InputType::Text, text));
	audio->waitEnd(portMAX_DELAY);
	return TickingState::Done;
}
