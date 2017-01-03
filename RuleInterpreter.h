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
    uint8_t pressedFor(uint8_t port, uint8_t pin, uint8_t ticks);
    uint8_t releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks);

private:
    Trigger::Enum triggerForTicks(uint8_t ticks);
    Trigger::Enum triggerType(uint8_t trigger);
	bool isRuleFor(uint8_t rule, uint8_t port, uint8_t pin) const;

    const unsigned char *m_rules;
    const int m_rulesSize;
};

#endif // RULEINTERPRETER_H
