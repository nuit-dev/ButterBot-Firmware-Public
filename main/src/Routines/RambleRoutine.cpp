#include "RambleRoutine.h"
#include <Statics/ApplicationStatics.h>
#include <Core/Application.h>
#include <Services/Audio/Audio.h>
#include <BBData.h>
#include <Phrases.h>
#include <cstdlib>
#include <Util/ServiceLocator.h>
#include "Audio/SpeechAudioGen.h"
#include "Audio/SpeechAudioSource.h"
#include "Audio/SpeechGen.h"
#include "Services/Com.h"
#include "Services/Time.h"
#include <QuoteText.h>

DEFINE_LOG(RambleRoutine)

Routine::TickingState RambleRoutine::tick(float deltaTime){
    const Application* app = ApplicationStatics::getApplication();
    Audio* audio = app->getService<Audio>();
    Com* com = app->getService<Com>();

    if(audio == nullptr || !ServiceLocator::SpeechAudioGenInstance || !ServiceLocator::SpeechGenInstance || com == nullptr){
        CMF_LOG(RambleRoutine, LogLevel::Error, "Missing required service(s)");
        abort();
    }

    // Custom (NUIT): when the clock is set, every third comment fits the time of day, and on Thursday some are
    // about the new strip (in TALKIE TOASTER mode, their Toaster versions).
    RambleKind kind = RambleKind::Ramble;
    const Time* timeService = app->getService<Time>();
    if(timeService != nullptr && timeService->isConfigured() && rand() % 3 == 0){
        const tm now = timeService->getTime();
        kind = (now.tm_wday == 4 && rand() % 2 == 0) ? RambleKind::Thursday : dayPeriod(now.tm_hour);
    }
    const Phrase phrase = rambleKindPhrase(kind);

    const int16_t id = Phrases::get(phrase);
    if(id < 0){
        CMF_LOG(RambleRoutine, LogLevel::Warning, "Phrases::get returned no phrase id");
        return TickingState::Done;
    }

    RambleData rambleData{};
    rambleData.id = id;
    rambleData.kind = static_cast<uint8_t>(kind);
    com->sendData(BB::State::Idle, BB::Action::Idle::Ramble, rambleData);

    auto source = std::make_unique<SpeechAudioSource>(SpeechGen::InputType::Text, Phrases::map(phrase, id));
    audio->play(ServiceLocator::SpeechAudioGenInstance.get(), std::move(source));
    audio->waitEnd(portMAX_DELAY);

    return TickingState::Done;
}
