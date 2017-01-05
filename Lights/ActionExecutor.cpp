#include "ActionExecutor.h"
#include "Rules.h"

ActionExecutor::ActionExecutor(McpExpanderGroup& expanders)
: m_expanders(expanders)
{}

void ActionExecutor::onTick() {
		// TODO maintain timers
}

void ActionExecutor::toggle(byte port, byte pin) {
	for (size_t i = 0; i < m_expanders.size(); ++i) {
		auto& expander = *(m_expanders.at(i));
		if (expander.address() == port) {
			Serial.print(F("Toggling "));
			Serial.print(port);
			Serial.print(' ');
			Serial.println(pin);
			expander.setOutputs(expander.outputs() ^ (1 << pin)); // move to expander togglePin method?
			return;
		}
	}
}

void ActionExecutor::timerReset(byte port, byte pin)
{}

void ActionExecutor::allOff()
{}

void ActionExecutor::perform(byte action) {
	switch (static_cast<Actions>(action >> SHIFT_ACTION)) {
	case Toggle:
		toggle(7 & (action >> SHIFT_PORT), action & 7);
		break;

	case TimerReset:
		timerReset(7 & (action >> SHIFT_PORT), action & 7);
		break;

	case AllOff:
		allOff();
		break;
	}
}
