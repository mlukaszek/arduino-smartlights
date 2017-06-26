#include "OutputSetter.h"
#include "Rules.h"

OutputSetter::OutputSetter(McpExpanderGroup& expanders)
: m_expanders(expanders)
, ticks(0)
{}

void OutputSetter::onTick() {
	if (++ticks < ms_to_ticks(1000)) {
		return;
	}

	ticks = 0;
	for (byte t = 0; t < MaxTimers; ++t) {
		auto& timer = timers.at(t);
		if (timer.counting && (0 == --timer.seconds)) {
			timer.counting = false;
			for (size_t i = 0; i < m_expanders.size(); ++i) {
				auto& expander = *(m_expanders.at(i));
				if (expander.address() == timer.address) {
					SerialTimestamp();
					Serial.print(F("Setting low "));
					SerialPrintPair(timer.address, timer.pin);
					expander.digitalWrite(timer.pin, LOW);
				}
			}
		}
	}
}

void OutputSetter::toggle(byte address, byte pin) {
	for (size_t i = 0; i < m_expanders.size(); ++i) {
		auto& expander = *(m_expanders.at(i));
		if (expander.address() == address) {
			SerialTimestamp();
			Serial.print(F("Toggling "));
			SerialPrintPair(address, pin, false);
			Serial.println(expander.toggleOutput(pin) == HIGH ? F(" On") : F(" Off"));
			return;
		}
	}
}

void OutputSetter::timerReset(byte address, byte pin)
{
	auto& timer = timers.at(0);
	for (byte i = 0; i < MaxTimers; ++i) {
		if ((timer.address == address && timer.pin == pin) || !timer.counting) {
			timer = timers.at(i);
			break;
		}
	}

	timer.address = address;
	timer.pin = pin;
	timer.seconds = 30;
	timer.counting = true;

	for (size_t i = 0; i < m_expanders.size(); ++i) {
		auto& expander = *(m_expanders.at(i));
		if (expander.address() == timer.address) {
			SerialTimestamp();
			Serial.print(F("Setting high "));
			SerialPrintPair(address, pin);
			expander.digitalWrite(timer.pin, HIGH);
		}
	}
}

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
