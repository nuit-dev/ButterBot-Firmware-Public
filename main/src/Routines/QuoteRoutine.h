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
 * follows along, and Shut Up stops the rest.
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
