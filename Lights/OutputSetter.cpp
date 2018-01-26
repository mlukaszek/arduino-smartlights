#include "OutputSetter.h"
#include "Rules.h"

OutputSetter::OutputSetter(McpExpanderGroup& expanders)
: m_expanders(expanders)
, ticks(0)
{}

void OutputSetter::onTick() {
	if (++ticks < ms_to_ticks(30000)) {
		return;
	}

	ticks = 0;
	for (byte t = 0; t < MaxTimers; ++t) {
		auto& timer = timers.at(t);
		if (timer.counting && (0 == --timer.halfmins)) {
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

void OutputSetter::timerReset(byte address, byte pin, byte context)
{
	auto& timer = timers.at(0);
	for (byte i = 0; i < MaxTimers; ++i) {
		// find a slot with existing running timer, or a free slot
		if ((timer.address == address && timer.pin == pin) || !timer.counting) {
			timer = timers.at(i);
			break;
		}
	}

	// TODO: if the timer was counting, act according to resetable flag

	timer.address = address;
	timer.pin = pin;
	timer.halfmins = context >> 1;
	timer.resetable = context & 1;
	timer.counting = true;

	for (size_t i = 0; i < m_expanders.size(); ++i) {
		auto& expander = *(m_expanders.at(i));
		if (expander.address() == timer.address) {
			SerialTimestamp();
			Serial.print(F("Timer set to "));
			Serial.println(timer.halfmins, DEC);
			Serial.print(F("Resetable? "));
			Serial.println(timer.resetable);
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

void OutputSetter::execute(Command command, byte context) {
	switch (command.effect) {
	case Toggle:
		toggle(command.address, command.output);
		break;

	case TimerReset:
		timerReset(command.address, command.output, context);
		break;

	case AllOff:
		allOff();
		break;
	}
}
