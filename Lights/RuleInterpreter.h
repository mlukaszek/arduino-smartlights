#ifndef RULEINTERPRETER_H
#define RULEINTERPRETER_H

#include "Common.h"

struct Trigger {
    enum Enum {
        None,
        ShortPress,
        MediumPress,
        LongPress
    };
};

class RuleInterpreter {
public:
    RuleInterpreter(const unsigned char* rules, int rulesSize);
    Command pressedFor(uint8_t port, uint8_t pin, uint8_t ticks) const;
    Command releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks) const;

private:
	bool findRuleFor(uint8_t port, uint8_t pin, uint8_t& offset) const;
	inline bool isRuleFor(uint8_t index, uint8_t port, uint8_t pin) const;

    Trigger::Enum triggerForTicks(uint8_t ticks) const;
    Trigger::Enum triggerType(uint8_t index) const;

    const unsigned char *m_rules;
    const int m_rulesSize;
};

#endif // RULEINTERPRETER_H
