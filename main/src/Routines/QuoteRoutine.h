#ifndef BUTTERBOT_FIRMWARE_QUOTEROUTINE_H
#define BUTTERBOT_FIRMWARE_QUOTEROUTINE_H

#include <string>
#include <vector>
#include <BBData.h>
#include <CtrlData.h>
#include "Routine.h"

/**
 * Custom (NUIT): says one random quote of a category (OVERKLOKING / BENDER / ULTRON menu items)
 * and shows it on the controller. Long quotes are spoken sentence by sentence, the controller
 * follows along, and Shut Up stops the rest. Character categories (Darth, Hawking, HAL, Daisy)
 * always use their own voice, regardless of the VOICE setting. Daisy (SHUTDOWN menu item) powers the robot off
 * when the song ends.
 */
class QuoteRoutine : public Routine {
public:
	QuoteRoutine(BBStateMachine* sm, QuoteData::Category category, BB::Action::Scenario scenario);
	~QuoteRoutine() override;

	TickingState tick(float deltaTime) override;

private:
	void onCommand(Ctrl::Command cmd);
	bool playPart(size_t index);

	const QuoteData::Category category;
	const BB::Action::Scenario scenario;

	bool started = false;
	bool aborted = false;
	int16_t id = -1;
	std::vector<std::string> parts;
	size_t nextPart = 0;
	bool split = false;
	bool voiceOverridden = false;
};

class DarthQuoteRoutine : public QuoteRoutine {
public:
	explicit DarthQuoteRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Darth, BB::Action::Scenario::DarthQuote){}
};

class HawkingQuoteRoutine : public QuoteRoutine {
public:
	explicit HawkingQuoteRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Hawking, BB::Action::Scenario::HawkingQuote){}
};

class HalQuoteRoutine : public QuoteRoutine {
public:
	explicit HalQuoteRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Hal, BB::Action::Scenario::HalQuote){}
};

class DaisySongRoutine : public QuoteRoutine {
public:
	explicit DaisySongRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Daisy, BB::Action::Scenario::DaisySong){}
};

class OverklokingQuoteRoutine : public QuoteRoutine {
public:
	explicit OverklokingQuoteRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Overkloking, BB::Action::Scenario::OverklokingQuote){}
};

class BenderQuoteRoutine : public QuoteRoutine {
public:
	explicit BenderQuoteRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Bender, BB::Action::Scenario::BenderQuote){}
};

class UltronQuoteRoutine : public QuoteRoutine {
public:
	explicit UltronQuoteRoutine(BBStateMachine* sm) : QuoteRoutine(sm, QuoteData::Category::Ultron, BB::Action::Scenario::UltronQuote){}
};

#endif //BUTTERBOT_FIRMWARE_QUOTEROUTINE_H
