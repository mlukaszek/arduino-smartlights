#ifndef PINMONITOR_H
#define PINMONITOR_H

#include "Common.h"
#include "McpExpander.h"

union InputState {
	uint8_t value;
	struct {
		unsigned counter : 7;
		unsigned grounded : 1;
	};
};

class RuleInterpreter;
class OutputSetter;

class InputMonitor {
public:
	explicit InputMonitor(McpExpanderGroup& expanders);
	void onTick(RuleInterpreter& ruleInterpreter, OutputSetter& executor);

    constexpr static uint8_t LargestCount = 0xFF >> 1;

private:
	InputState inputs[McpExpander::PinsInPort * McpExpander::MaxCount];
	McpExpanderGroup& expanders;
};


#endif // PINMONITOR_H