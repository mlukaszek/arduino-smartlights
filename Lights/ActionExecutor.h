#pragma once
#include "Common.h"
#include "McpExpander.h"

class ActionExecutor
{
public:
	explicit ActionExecutor(McpExpanderGroup& expanders);
	void onTick();
	void toggle(byte port, byte pin);
	void timerReset(byte port, byte pin);
	void allOff();
	void perform(byte action);

private:
	McpExpanderGroup& m_expanders;
};

