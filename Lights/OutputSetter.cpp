#include "OutputSetter.h"
#include "Rules.h"

OutputSetter::OutputSetter(McpExpanderGroup& expanders)
: m_expanders(expanders)
{}

void OutputSetter::onTick() {
		// TODO maintain timers
}

void OutputSetter::toggle(byte port, byte pin) {
	for (size_t i = 0; i < m_expanders.size(); ++i) {
		auto& expander = *(m_expanders.at(i));
		if (expander.address() == port) {
			Serial.print(F("Toggling "));
			Serial.print(port);
			Serial.print(':');
			Serial.println(pin);
			expander.setOutputs(expander.outputs() ^ (1 << pin)); // TODO: move to expander togglePin method?
			return;
		}
	}
}

void OutputSetter::timerReset(byte port, byte pin)
{}

void OutputSetter::allOff()
{
	for (size_t i = 0; i < m_expanders.size(); ++i) {
		(m_expanders.at(i))->setOutputs(McpExpander::OutputsOff);
	}
}

void OutputSetter::execute(Command command) {
	switch (command.effect) {
	case Toggle:
		toggle(command.address, command.output);
		break;

	case TimerReset:
		timerReset(command.address, command.output);
		break;

	case AllOff:
		allOff();
		break;
	}
}
