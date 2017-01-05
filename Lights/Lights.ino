#include <LowPower.h>
#include "ActionExecutor.h"
#include "McpExpander.h"
#include "Rules.h"
#include "PinMonitor.h"
#include "RuleInterpreter.h"

static McpExpanderGroup expanders;
static ActionExecutor executor(expanders);
static RuleInterpreter ruleInterpreter(rules, rulesSize);

static PinMonitor pinMonitor;

void setup()
{
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	Serial.begin(9600);
	while (!Serial);
	Serial.println(F("Booting up"));

	static McpExpander expander1(0);
	static McpExpander expander4(4);
	expanders.add(expander1);
	expanders.add(expander4);
	Serial.print(F("Expanders configured: "));
	Serial.println(expanders.size());

	for (size_t i = 0; i < expanders.size(); ++i ) {
		expanders.at(i)->begin();
	}

	pinMonitor.begin(expanders.size());

	Serial.println(F("Boot complete"));
	digitalWrite(13, HIGH);
}

void scanInputs()
{
	for (size_t i = 0; i < expanders.size(); ++i) {
		auto& expander = *(expanders.at(i));
		expander.scanInputs();
		const uint8_t diff = expander.previousInputs() ^ expander.currentInputs();
		uint8_t action = 0;
		for (uint8_t pin = 0; pin<8; ++pin) {
			if (1 & (diff >> pin)) {
				if (1 & (expander.currentInputs() >> pin)) {
					action = pinMonitor.onPinChange(expander.address(), pin, High, ruleInterpreter);
				}
				else {
					action = pinMonitor.onPinChange(expander.address(), pin, Low, ruleInterpreter);
				}
				if (action) {
					Serial.print(F("Action from pin monitor onPinChange: "));
					Serial.println(action);
					executor.perform(action);
				}
			}
		}
	}
}

void tick()
{
	scanInputs();
	uint8_t action = 0; // pinMonitor.onTick(ruleInterpreter);
	if (action) {
		Serial.print(F("Action from pin monitor onTick: "));
		Serial.println(action);
		executor.perform(action);
	}
	executor.onTick();
}

void loop()
{
	tick();
	if (Serial.available() > 2) {
		int cmd = Serial.read();
		int port = Serial.read() - '0';
		int pin = Serial.read() - '0';

		String log(F("Command "));
		log += (char)cmd;
		log += ':';
		log += port;
		log += ',';
		log += pin;
		Serial.println(log);
		if (port >= 0 && pin >= 0 && pin <= 7 && port <= 7) {
			switch (cmd) {
			case 't': case 'T':
				executor.perform(port << 3 | pin);
				break;

			case 'r': case 'R':
				executor.perform(1 << 6 | port << 3 | pin);
				break;
			}
		}
	}
	LowPower.idle(SLEEP_60MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF);
}
