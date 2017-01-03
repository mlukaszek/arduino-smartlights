#include "RuleInterpreter.h"
#include "Common.h"
#ifdef ARDUINO
#include <Arduino.h>
#include <avr/pgmspace.h>
#endif

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

uint8_t RuleInterpreter::pressedFor(uint8_t port, uint8_t pin, uint8_t ticks)
{
    if (!pgm_read_byte_near(m_rules)) {
        return 0;
    }

	uint8_t i = 0;
    uint8_t count = m_rulesSize;
    Trigger::Enum trigger = triggerForTicks(ticks);
    // Skip rules which do not apply for this port/pin pair
    while (--count && !isRuleFor(i, port, pin)) {
        i += 2;
    }

    // Only take action if first found rule matches the searched trigger exactly.
    // If there are more rules, then the decision is deferred to releasedAfter handler.
    if (trigger == triggerType(i)) {
        return pgm_read_byte_near(m_rules + i + 1);
    }

    return 0;
}

uint8_t RuleInterpreter::releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks)
{
	if (!pgm_read_byte_near(m_rules)) {
		return 0;
	}

	uint8_t i = 0;
    uint8_t count = m_rulesSize;
    Trigger::Enum trigger = triggerForTicks(ticks);

    // Skip rules which do not match this port/pin pair
    while (--count && !isRuleFor(i, port, pin)) {
        i += 2;
    }

    // Find the right action for the trigger
    while (count-- && isRuleFor(i, port, pin)) {
        const bool exactMatch = triggerType(i) == trigger;
        const bool vagueMatch = Trigger::ShortPress == triggerType(i) && Trigger::MediumPress == trigger;
        if (exactMatch || vagueMatch) {
			return pgm_read_byte_near(m_rules + i + 1);
        }
        i += 2;
    }

    return 0;
}
