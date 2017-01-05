#pragma once
#include <Adafruit_MCP23017.h>
#include "Containers.h"

class McpExpander
{
public:
	constexpr static byte MAX_COUNT = 8;

	explicit McpExpander(byte addr)
		: m_address(addr)
		, m_current(0xFF)
		, m_previous(0xFF)
		, m_outputs(0)
	{}

	void begin() {
		mcp.begin(m_address);
		mcp.writeRegister(MCP23017_GPPUA, 0xFF);  // pullups on A
		mcp.writeRegister(MCP23017_IODIRB, 0);    // outputs on B
		Serial.print(F("Init expander with address "));
		Serial.println(m_address);
		Serial.flush();
	}

	void scanInputs() {
		m_previous = m_current;
		m_current = mcp.readGPIO(0);
	}

	void setOutputs(byte value) {
		m_outputs = value;
		mcp.writeRegister(MCP23017_GPIOB, value);
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
	Adafruit_MCP23017 mcp;
	byte m_address;
	byte m_current;
	byte m_previous;
	byte m_outputs;
};

typedef StaticListOfPointersTo<McpExpander, McpExpander::MAX_COUNT> McpExpanderGroup;