#ifndef RULEINTERPRETER_H
#define RULEINTERPRETER_H

#include "Common.h"
#include "Rules.h"

class RulesSource {
public:
	virtual ~RulesSource() {}
	virtual byte readByte(byte offset = 0) const = 0;
	virtual unsigned size() const = 0;
};

#ifdef __AVR__
class RulesFromFlash : public RulesSource {
public:
	explicit RulesFromFlash(const unsigned char* rules, unsigned size);
	virtual byte readByte(byte offset) const;
	virtual unsigned size() const { return m_size; }
private:
	const unsigned char *m_rules;
	const unsigned m_size;
};
#endif

class RuleInterpreter {
public:
    RuleInterpreter(const RulesSource& rulesSource);

	Command pressedFor(uint8_t port, uint8_t pin, uint8_t ticks);
	Command releasedAfter(uint8_t port, uint8_t pin, uint8_t ticks);
	byte context() const { return m_context; }

private:
	Command evaluate(uint8_t port, uint8_t pin, uint8_t ticks, bool failfast);
	void store(byte value) {
		m_context = value;
	}

    Trigger triggerForTicks(uint8_t ticks) const;
    Trigger triggerType(uint8_t index) const;
	const RulesSource &m_rulesSource;
	byte m_context;
};

#endif // RULEINTERPRETER_H
