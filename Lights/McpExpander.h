#pragma once
#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif
#include <Adafruit_MCP23017.h>
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
		mcp.begin(m_address);
		writeRegister(MCP23017_GPPUA, 0xFF);  // pullups on A
		writeRegister(MCP23017_IODIRB, 0);    // outputs on B
	}

	uint8_t scanInputs() {
		m_previous = m_current;
		m_current = mcp.readGPIO(0);
		return m_previous ^ m_current;
	}

	void setOutputs(byte value) {
		m_outputs = value;
		writeRegister(MCP23017_GPIOB, value);
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
	void writeRegister(byte regAddr, byte regValue) {
		Wire.beginTransmission(MCP23017_ADDRESS | m_address);
		Wire.write((uint8_t)regAddr);
		Wire.write((uint8_t)regValue);
		Wire.endTransmission();
	}

	McpExpander(McpExpander&) = delete;
	Adafruit_MCP23017 mcp;
	byte m_address;
	byte m_current;
	byte m_previous;
	byte m_outputs;
};

typedef PtrArray<McpExpander, McpExpander::MaxCount> McpExpanderGroup;