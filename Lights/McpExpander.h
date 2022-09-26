#pragma once
#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif
#include <Adafruit_MCP23X17.h>
#include "Containers.h"

class McpExpander
{
public:
	constexpr static byte MaxCount = 8;
	constexpr static byte PinsInPort = 8;
	constexpr static byte OutputsOff = 0;

	explicit McpExpander(byte addr)
		: m_address(addr)
		, m_current(0xFF)
		, m_previous(0xFF)
		, m_outputs(OutputsOff)
	{}

	void begin() {
		mcp.begin_I2C(MCP23XXX_ADDR | m_address);
		for (auto pin = 0; pin < 8; pin++) {
			mcp.pinMode(pin, INPUT_PULLUP);
		}
		for (auto pin = 8; pin < 16; pin++) {
			mcp.pinMode(pin, OUTPUT);
		}
	}

	uint8_t scanInputs() {
		m_previous = m_current;
		m_current = mcp.readGPIO(0);
		return m_previous ^ m_current;
	}

	void setOutputs(byte value) {
		m_outputs = value;
		mcp.writeGPIO(value, 1);
	}

	byte toggleOutput(byte pin) {
		setOutputs(outputs() ^ (1 << pin));
		return mcp.digitalRead(8 + pin);
	}

	void digitalWrite(byte pin, byte value) {
		if (value) {
			setOutputs(outputs() | (1 << pin));
		}
		else {
			setOutputs(outputs() & ~(1 << pin));
		}
	}

	inline byte currentInputs() const {
		return m_current;
	}

	inline byte previousInputs() const {
		return m_previous;
	}

	inline byte address() const {
		return m_address;
	}

	inline byte outputs() const {
		return m_outputs;
	}

private:
	McpExpander(McpExpander&) = delete;
	Adafruit_MCP23X17 mcp;
	byte m_address;
	byte m_current;
	byte m_previous;
	byte m_outputs;
};

typedef PtrArray<McpExpander, McpExpander::MaxCount> McpExpanderGroup;

class McpExpanderGroupDiscoverer {
public:
	explicit McpExpanderGroupDiscoverer(McpExpanderGroup& expanders) {
		Wire.begin();
		byte error, address;
		for (address = 0; address < McpExpander::MaxCount; ++address) {
			Wire.beginTransmission(MCP23XXX_ADDR | address);
			error = Wire.endTransmission();
			if (0 == error) {
				Serial.print("Found expander with address ");
				Serial.println(address);
				expanders.add(new McpExpander(address));
				expanders.last()->begin();
			}
		}
	}
};
