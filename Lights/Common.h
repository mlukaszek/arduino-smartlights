#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#ifdef ARDUINO
#include "Arduino.h"
#endif

constexpr uint8_t msPerTick = 15;

constexpr uint16_t ms_to_ticks(uint16_t ms) {
    return ms / msPerTick;
}

enum PressingTime {
    // in milliseconds
    ShortPressMs = 30,
    MediumPressMs = 600,
    LongPressMs = 3000
};

union Rule {
	uint8_t value;
	struct {
		unsigned input : 3;
		unsigned address : 3;
		unsigned trigger : 2;
	};
};

union Command {
	uint8_t value;
	struct {
		unsigned output : 3;
		unsigned address : 3;
		unsigned effect : 2;
	};
};

constexpr Command noop = { 0 };

inline void SerialPrintPair(byte a, byte b, bool eol=true) {
	Serial.print(a);
	Serial.print(':');
	if (eol) {
		Serial.println(b);
	}
	else {
		Serial.print(b);
	}
}

inline void SerialTimestamp() {
	Serial.print(millis());
	Serial.print(' ');
}

#endif // COMMON_H