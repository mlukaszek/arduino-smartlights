#pragma once
#include "Common.h"
#include "Containers.h"
#include "McpExpander.h"

union Timer
{
	uint16_t value;
	struct {
		unsigned counting : 1;
		unsigned seconds : 9;
		unsigned address : 3;
		unsigned pin : 3;
	};
};

class OutputSetter
{
public:
	constexpr static byte MaxTimers = 8;

	explicit OutputSetter(McpExpanderGroup& expanders);
	void onTick();
	void toggle(byte address, byte pin);
	void timerReset(byte address, byte pin);
	void allOff();
	void execute(Command command);

private:
	uint16_t ticks;
	McpExpanderGroup& m_expanders;
	Array<Timer, MaxTimers> timers;
};

