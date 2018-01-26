#pragma once
#include "Common.h"
#include "Containers.h"
#include "McpExpander.h"

union Timer
{
	uint16_t value;
	struct {
		unsigned counting : 1;
		unsigned resetable : 1;
		unsigned address : 3;
		unsigned pin : 3;
		unsigned halfmins : 8;
	};
};

class OutputSetter
{
public:
	constexpr static byte MaxTimers = 8;

	explicit OutputSetter(McpExpanderGroup& expanders);
	void onTick();
	void toggle(byte address, byte pin);
	void timerReset(byte address, byte pin, byte context);
	void allOff();
	void execute(Command command, byte context);

private:
	void set(byte address, byte pin, bool on);
	void updateTimer(Timer& timer, byte context);
	void setupTimer(Timer& timer, byte address, byte pin, byte context);

	McpExpanderGroup& m_expanders;
	uint16_t ticks;
	Array<Timer, MaxTimers> timers;
};

