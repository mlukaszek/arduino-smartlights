#include "RuleInterpreter.h"
#include "Common.h"
#include <avr/pgmspace.h>

namespace {
	constexpr byte NO_ACTION = 0;
}

RuleInterpreter::RuleInterpreter(const unsigned char *rules, int rulesSize)
: m_rules(rules)
, m_rulesSize(rulesSize)
{}

Trigger::Enum RuleInterpreter::triggerForTicks(uint8_t ticks)
{
    if (ticks >= ms_to_ticks(LongPressMs)) {
        return Trigger::LongPress;
    }
    if (ticks >= ms_to_ticks(MediumPressMs)) {
        return Trigger::MediumPress;
    }
    if (ticks >= ms_to_ticks(ShortPressMs)) {
        return Trigger::ShortPress;
    }

    return Trigger::None;
}

Trigger::Enum RuleInterpreter::triggerType(uint8_t index)
{
	const byte trigger = pgm_read_byte_near(m_rules + index);
    if (trigger >> 7) {
        return Trigger::LongPress;
    }
    if (trigger >> 6) {
        return Trigger::MediumPress;
    }
    if (0 == trigger >> 6) {
        return Trigger::ShortPress;
    }
    return Trigger::None;
}

bool RuleInterpreter::isRuleFor(uint8_t index, uint8_t port, uint8_t pin) const
{
	return (pgm_read_byte_near(m_rules + index) & 0x3F) == (port << 3 | pin);
}

bool RuleInterpreter::findRuleFor(uint8_t port, uint8_t pin, uint8_t& offset) const
{
	int i = 0;
	if (!pgm_read_byte_near(m_rules)) {
		return NO_ACTION;
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

uint8_t RuleInterpreter::pressedFor(uint8_t port, uint8_t pin, uint8_t ticks)
{
	// Only take action if first found rule matches the searched trigger exactly.
	// If there are more rules, then the decision is deferred to releasedAfter handler.
	uint8_t i = 0;
	if (findRuleFor(port, pin, i) && triggerForTicks(ticks) == triggerType(i)) {
		return pgm_read_byte_near(m_rules + i + 1);
	}
	return NO_ACTION;
}

uint8_t RuleInterpreter::releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks)
{
	const auto trigger = triggerForTicks(ticks);
	uint8_t i = 0;

	if (findRuleFor(port, pin, i)) {
		do {
			// Iterate until the rule matches the trigger (correct length of the press)
			const bool exactMatch = triggerType(i) == trigger;
			const bool vagueMatch = Trigger::ShortPress == triggerType(i) && Trigger::MediumPress == trigger;
			if (exactMatch || vagueMatch) {
				return pgm_read_byte_near(m_rules + i + 1);
			}
			else {
				i += 2;
			}
		} while (isRuleFor(i, port, pin));
	}
	return NO_ACTION;
}
