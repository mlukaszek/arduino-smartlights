#include <pins_arduino.h>
#include <LowPower.h>
#include "McpExpander.h"
#include "InputMonitor.h"
#include "OutputSetter.h"
#include "Rules.h"
#include "RuleInterpreter.h"

static McpExpanderGroup expanders;
static InputMonitor inputMonitor(expanders);
static OutputSetter outputSetter(expanders);
static RuleInterpreter ruleInterpreter(rules, rulesSize);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
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

	Serial.println(F("Boot complete"));
	digitalWrite(LED_BUILTIN, HIGH);
}

void handleSerialInput()
{
	if ('?' == Serial.peek()) {
		Serial.read();
		for (byte i = 0; i < expanders.size(); ++i) {
			Serial.print(expanders.at(i)->address());
			Serial.print('=');
			byte outputs = expanders.at(i)->outputs();
			Serial.print(outputs);
			Serial.print(" (");
			Serial.print(outputs, BIN);
			Serial.println(')');
		}
	}

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
				outputSetter.toggle(port, pin);
				break;

			case 'r': case 'R':
				outputSetter.timerReset(port, pin);
				break;
			}
		}
	}
}

void loop()
{
	// handle inputs (holding/releasing buttons)
	inputMonitor.onTick(ruleInterpreter, outputSetter);

	// handle timers that may expire
	outputSetter.onTick();

	handleSerialInput();

	LowPower.idle(SLEEP_60MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF, SPI_OFF, USART0_ON, TWI_OFF);
}
