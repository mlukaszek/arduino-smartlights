#include <pins_arduino.h>

#ifdef ESP8266
#include <pgmspace.h>
constexpr byte LED_ON = LOW;
constexpr byte LED_OFF = HIGH;
#else
#include <Adafruit_SleepyDog.h>
constexpr byte LED_ON = HIGH;
constexpr byte LED_OFF = LOW;
#endif

#include "McpExpander.h"
#include "InputMonitor.h"
#include "OutputSetter.h"
#include "Rules.h"
#include "RuleInterpreter.h"

static McpExpanderGroup expanders;
static InputMonitor inputMonitor(expanders);
static OutputSetter outputSetter(expanders);
static RuleInterpreter ruleInterpreter(rules, rulesSize);
static uint8_t loops(0);

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(9600);
	while (!Serial);

	Wire.begin();
	byte error, address;
	for (address = 0; address < McpExpander::MaxCount; ++address) {
		Wire.beginTransmission(MCP23017_ADDRESS | address);
		error = Wire.endTransmission();
		if (0 == error) {
			Serial.print("Found expander with address ");
			Serial.println(address);
		}
	}

	static McpExpander expander0(0);
	static McpExpander expander4(4);
	//static McpExpander expander1(1);
	//static McpExpander expander2(2);
	//static McpExpander expander3(3);
	expanders.add(expander0);
	expanders.add(expander4);
	//expanders.add(expander1);
	//expanders.add(expander2);
	//expanders.add(expander3);

	for (size_t i = 0; i < expanders.size(); ++i) {
		expanders.at(i)->begin();
	}

	Serial.println(F("Boot complete"));

#ifdef ESP8266
	ESP.wdtEnable(WDTO_1S);
#else
	Watchdog.enable(1000);
#endif
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
	// handle timers that may expire
	outputSetter.onTick();

	handleSerialInput();

	// handle inputs (holding/releasing buttons)
	if (inputMonitor.onTick(ruleInterpreter, outputSetter)) {
		// if all inputs are stable, sleep for a bit
		delay(15);

		// Blink led every so often
		if (0 == ++loops % 200) {
			digitalWrite(LED_BUILTIN, LED_ON);
			loops = 0;
		}
		else {
			digitalWrite(LED_BUILTIN, LED_OFF);
		}
	}

#ifdef ESP8266
	ESP.wdtFeed();
#else
	Watchdog.reset();
#endif
}
