#include "OutputSetter.h"
#include "Rules.h"

OutputSetter::OutputSetter(McpExpanderGroup& expanders)
: m_expanders(expanders)
, ticks(0)
{}

void OutputSetter::onTick() {
	constexpr uint16_t halfmin_in_ms = 30000;

	if (++ticks < ms_to_ticks(halfmin_in_ms)) {
		return;
	}

	// every half minute see if timers expired
	ticks = 0;
	for (byte t = 0; t < MaxTimers; ++t) {
		auto& timer = timers.at(t);
		if (timer.counting && (0 == --timer.halfmins)) {
			timer.counting = false;
			set(timer.address, timer.pin, false);
		}
	}
}

void OutputSetter::set(byte address, byte pin, bool on) {
	for (size_t i = 0; i < m_expanders.size(); ++i) {
		auto& expander = *(m_expanders.at(i));
		if (expander.address() == address) {
			SerialTimestamp();
			Serial.print(F("Setting "));
			Serial.print(on? F("high ") : F("low "));
			SerialPrintPair(address, pin);
			expander.digitalWrite(pin, on? HIGH : LOW);
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

void OutputSetter::updateTimer(Timer& timer, byte context)
{
	SerialTimestamp();
	Serial.print(F("Running timer for "));
	SerialPrintPair(timer.address, timer.pin, false);

	if (timer.resetable) {
		timer.halfmins = context >> 1;
		ticks = 0;
		Serial.print(F(" reset to "));
		Serial.println(timer.halfmins);
	}
	else {
		Serial.println(F(" cancelled"));
		timer.counting = 0;
		set(timer.address, timer.pin, false);
	}
}

void OutputSetter::setupTimer(Timer& timer, byte address, byte pin, byte context)
{
	timer.address = address;
	timer.pin = pin;
	timer.halfmins = context >> 1;
	timer.resetable = context & 1;
	timer.counting = true;

	SerialTimestamp();
	Serial.print(timer.resetable ? F("Resettable ") : F("Cancellable "));
	Serial.print(F("timer set to "));
	Serial.print(timer.halfmins);
	Serial.print(" for ");
	SerialPrintPair(address, pin);
	set(timer.address, timer.pin, true);
}

void OutputSetter::timerReset(byte address, byte pin, byte context)
{
	auto& timer = timers.at(0);
	for (byte i = 0; i < MaxTimers; ++i) {
		// find a slot with existing running timer, or a free slot
		auto& current = timers.at(i);
		if ((current.address == address && current.pin == pin) || !current.counting) {
			timer = current;
			break;
		}
	}

	if (timer.counting) {
		updateTimer(timer, context);
	}	else {
		setupTimer(timer, address, pin, context);
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
