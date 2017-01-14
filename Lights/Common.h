#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#ifdef ARDUINO
#include "Arduino.h"
#endif

constexpr uint8_t msPerTick = 60;

inline uint16_t ms_to_ticks(uint16_t ms) {
    return ms / msPerTick;
}

enum PressingTime {
    // in milliseconds
    ShortPressMs = 60,
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

inline void SerialPrintPair(byte a, byte b) {
	Serial.print(a);
	Serial.print(':');
	Serial.println(b);
}

#endif // COMMON_H