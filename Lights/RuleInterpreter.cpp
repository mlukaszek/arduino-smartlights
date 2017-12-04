#include "RuleInterpreter.h"
#include "Common.h"
#ifdef ESP8266
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif

RuleInterpreter::RuleInterpreter(const unsigned char *rules, int rulesSize)
: m_rules(rules)
, m_rulesSize(rulesSize)
{}

Trigger RuleInterpreter::triggerForTicks(uint8_t ticks) const
{
    if (ticks >= ms_to_ticks(LongPressMs)) {
        return LongPress;
    }
    if (ticks >= ms_to_ticks(MediumPressMs)) {
        return MediumPress;
    }
    if (ticks >= ms_to_ticks(ShortPressMs)) {
        return ShortPress;
    }
    return None;
}

Trigger RuleInterpreter::triggerType(uint8_t index) const
{
	Rule rule{ pgm_read_byte_near(m_rules + index) };
	return static_cast<Trigger>(rule.trigger);
}

bool RuleInterpreter::isRuleFor(uint8_t index, uint8_t port, uint8_t pin) const
{
	Rule rule{ pgm_read_byte_near(m_rules + index) };
	return rule.address == port && rule.input == pin;
}

bool RuleInterpreter::findRuleFor(uint8_t port, uint8_t pin, uint8_t& offset) const
{
	int i = 0;
	if (!pgm_read_byte_near(m_rules)) {
		return false;
	}

	uint8_t count = m_rulesSize;
	// Skip rules which do not apply for this port/pin pair
	while (--count && !isRuleFor(i, port, pin)) {
		i += 2;
	}

	if (count) {
		offset = i;
	}
	return count;
}

Command RuleInterpreter::pressedFor(uint8_t port, uint8_t pin, uint8_t ticks) const
{
	// Only take action if first found rule matches the searched trigger exactly.
	// If there are more rules, then the decision is deferred to releasedAfter handler.
	uint8_t i = 0;
	if (findRuleFor(port, pin, i) && triggerForTicks(ticks) == triggerType(i)) {
		return Command{ pgm_read_byte_near(m_rules + i + 1) };
	}
	return noop;
}

Command RuleInterpreter::releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks) const
{
	const auto trigger = triggerForTicks(ticks);
	uint8_t i = 0;

	if (findRuleFor(port, pin, i)) {
		do {
			// Iterate until the rule matches the trigger (correct length of the press)
			const bool exactMatch = triggerType(i) == trigger;
			const bool vagueMatch = Trigger::ShortPress == triggerType(i) && Trigger::MediumPress == trigger;
			if (exactMatch || vagueMatch) {
				return Command{ pgm_read_byte_near(m_rules + i + 1) };
			}
			else {
				i += 2;
			}
		} while (isRuleFor(i, port, pin));
	}
	return noop;
}
