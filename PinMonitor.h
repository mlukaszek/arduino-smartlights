#ifndef PINMONITOR_H
#define PINMONITOR_H

#include "Common.h"

static const uint8_t maxSupportedPorts = 8;

enum LogicalState {
	Low,
	High
};

class RuleInterpreter;
class ActionExecutor;

class PinMonitor {
public:
	PinMonitor(uint8_t count);
	uint8_t onPinChange(uint8_t port, uint8_t pin, LogicalState changedTo, RuleInterpreter& ruleInterpreter);
	uint8_t onTick(RuleInterpreter& ruleInterpreter);

    const static uint8_t largestCount = 0xFF >> 1;
    const static uint8_t stateMask = 1 << 7;
    const static uint8_t countMask = largestCount;
    static uint8_t m_count;

protected: // for tests
    uint8_t state[maxSupportedPorts];
};


#endif // PINMONITOR_H