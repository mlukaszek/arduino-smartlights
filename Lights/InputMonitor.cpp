#include "InputMonitor.h"
#include "RuleInterpreter.h"
#include "OutputSetter.h"

namespace {
	inline bool isBitHigh(byte value, byte bit) {
		return (1 & (value >> bit));
	}
}

InputMonitor::InputMonitor(McpExpanderGroup& expanders)
: inputs{ 0 } // grounded=0, counter=0
, expanders(expanders)
{}

bool InputMonitor::onTick(RuleInterpreter& ruleInterpreter, OutputSetter& outputSetter)
{
	bool inputsStable = true;
	for (byte i = 0; i < expanders.size(); ++i) {
		auto& expander = *(expanders.at(i));

		const byte diff = expander.scanInputs();
		const byte current = expander.currentInputs();

		for (auto pin = 0; pin < McpExpander::PinsInPort; ++pin) {
			Command command;
			auto& input = inputs[i * McpExpander::PinsInPort + pin];

			// Has pin changed state since the last time?
			if (isBitHigh(diff, pin)) {
				Serial.print(F("Input "));
				Serial.print(expander.address());
				Serial.print(":");
				Serial.print(pin);

				const bool changedToHigh = isBitHigh(current, pin);
				Serial.println(changedToHigh? F(" High") : F(" Low"));
				Serial.flush();

				if (changedToHigh && input.counter && input.counter < LargestCount) {
					command = ruleInterpreter.releasedAfter(expander.address(), pin, input.counter);
					if (command.effect) {
						outputSetter.execute(command);
					}
				}

				// Store the state for next time and reset the counter
				input.grounded = !changedToHigh;
				input.counter = 0;
				inputsStable = false;
				continue;
			}

			// Not changed, manage counter state
			if (input.counter < LargestCount) {
				++input.counter;

				if (input.grounded) {
					command = ruleInterpreter.pressedFor(expander.address(), pin, input.counter);
					if (command.effect) {
						// Prevent increasing the counter more and execute the requested command
						input.counter = LargestCount;
						outputSetter.execute(command);
					}
				}
			}
		}		
	}
	return inputsStable;
}