#include "PinMonitor.h"
#include "RuleInterpreter.h"
#include "ActionExecutor.h"
#ifdef ARDUINO
# include <Arduino.h>
#endif

uint8_t PinMonitor::m_count = 0;

PinMonitor::PinMonitor(uint8_t count)
{
    m_count = count;
    for (uint8_t i = 0; i < count; ++i) {
        state[i] = stateMask; // counter=0, state is High
	}
}

uint8_t PinMonitor::onTick(RuleInterpreter& ruleInterpreter)
{
    for (uint8_t i = 0; i < m_count; ++i) {
        const uint8_t tickCount = state[i] & countMask;
        if (tickCount < largestCount) {
            ++state[i];

            // keep reporting to ruleInterpreter until counter stops increasing
            if (0 == state[i] >> 7) {
                uint8_t action = ruleInterpreter.pressedFor(i / pinsInPort, i % pinsInPort, tickCount + 1);
                if (action) {
                    // don't increase the counter anymore and execute the requested action
                    state[i] = largestCount;
                    return action; // will exit to execute first encountered action
                }
            }
        }
    }
}

uint8_t PinMonitor::onPinChange(uint8_t port, uint8_t pin, LogicalState changedTo, RuleInterpreter& ruleInterpreter)
{
#ifdef ARDUINO
    Serial.print(F("Pin change "));
    Serial.print(port);
    Serial.print(":");
    Serial.println(pin);
    Serial.flush();
#endif
    // if turning High, report for how many ticks was it Low
	uint8_t action = 0;
    if (High == changedTo) {
       const uint8_t tickCount = state[port * pinsInPort + pin] & countMask;
       if (tickCount && tickCount != largestCount) {
           action = ruleInterpreter.releasedAfter(port, pin, tickCount);
       }
    }

    state[port * pinsInPort + pin] = (changedTo == Low) ? 0 : stateMask;
	return action;
}
