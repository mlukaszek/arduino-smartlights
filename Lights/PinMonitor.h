#ifndef PINMONITOR_H
#define PINMONITOR_H

#include "Common.h"
#include "McpExpander.h"

enum LogicalState {
	Low,
	High
};

class RuleInterpreter;
class ActionExecutor;

class PinMonitor {
public:
	PinMonitor();
	void begin(uint8_t count);
	uint8_t onPinChange(uint8_t port, uint8_t pin, LogicalState changedTo, RuleInterpreter& ruleInterpreter);
	uint8_t onTick(RuleInterpreter& ruleInterpreter);

    const static uint8_t largestCount = 0xFF >> 1;
    const static uint8_t stateMask = 1 << 7;
    const static uint8_t countMask = largestCount;

protected: // for tests
    uint8_t state[McpExpander::MAX_COUNT];

private:
	uint8_t m_count;
};


#endif // PINMONITOR_H