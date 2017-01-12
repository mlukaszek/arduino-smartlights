#pragma once
#include "Common.h"
#include "McpExpander.h"



class OutputSetter
{
public:
	explicit OutputSetter(McpExpanderGroup& expanders);
	void onTick();
	void toggle(byte port, byte pin);
	void timerReset(byte port, byte pin);
	void allOff();
	void execute(Command command);

private:
	McpExpanderGroup& m_expanders;
};

