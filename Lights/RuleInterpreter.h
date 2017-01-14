#ifndef RULEINTERPRETER_H
#define RULEINTERPRETER_H

#include "Common.h"
#include "Rules.h"

class RuleInterpreter {
public:
    RuleInterpreter(const unsigned char* rules, int rulesSize);
    Command pressedFor(uint8_t port, uint8_t pin, uint8_t ticks) const;
    Command releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks) const;

private:
	bool findRuleFor(uint8_t port, uint8_t pin, uint8_t& offset) const;
	inline bool isRuleFor(uint8_t index, uint8_t port, uint8_t pin) const;

    Trigger triggerForTicks(uint8_t ticks) const;
    Trigger triggerType(uint8_t index) const;

    const unsigned char *m_rules;
    const int m_rulesSize;
};

#endif // RULEINTERPRETER_H
