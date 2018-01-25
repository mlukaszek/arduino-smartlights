#include "RuleInterpreter.h"
#include "Common.h"

#ifdef ARDUINO

#ifdef __AVR__
# include <avr/pgmspace.h>
#elif defined(ESP8266)
# include <pgmspace.h>
#endif

RulesFromFlash::RulesFromFlash(const unsigned char* rules, unsigned size)
	: m_rules(rules)
	, m_size(size)
{}

byte RulesFromFlash::readByte(byte offset) const
{
	return offset < m_size ? pgm_read_byte_near(m_rules + offset) : 0;
}
#endif

RuleInterpreter::RuleInterpreter(const RulesSource& rulesSource)
: m_rulesSource(rulesSource)
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

Trigger RuleInterpreter::triggerType(uint8_t val) const
{
	return static_cast<Trigger>(val);
}

Command RuleInterpreter::evaluate(uint8_t port, uint8_t pin, uint8_t ticks, bool failfast)
{
	if (0 == m_rulesSource.size()) {
		return noop;
	}

	uint8_t i = 0;

	while (i < m_rulesSource.size()) {
		Rule rule{ m_rulesSource.readByte(i++) };

		if (rule.trigger == None) {
			i++; // ignore next byte
			continue;
		}

		Command command{ m_rulesSource.readByte(i++) };
		if (0 == command.value) {
			break;
		}

		while (command.effect == Store) {
			store(command.value);
			command.value = m_rulesSource.readByte(i++);
		}

		if (rule.address == port && rule.input == pin) {
			const bool perfectMatch = triggerType(rule.trigger) == triggerForTicks(ticks);
			if (!perfectMatch && failfast) {
				return noop;
			}

			const bool vagueMatch = ShortPress == triggerType(rule.trigger) && MediumPress == triggerForTicks(ticks);
			if (perfectMatch || vagueMatch) {
				return command;
			}
		}
	}

	return noop;
}


Command RuleInterpreter::pressedFor(uint8_t port, uint8_t pin, uint8_t ticks)
{
	return evaluate(port, pin, ticks, true);
}

Command RuleInterpreter::releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks)
{
	return evaluate(port, pin, ticks, false);
}
