#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

const uint8_t pinsInPort = 8;
const uint8_t msPerTick = 60;

inline uint16_t ms_to_ticks(uint16_t ms) {
    return ms / msPerTick;
}

enum PressingTime {
    // in milliseconds
    ShortPressMs = 60,
    MediumPressMs = 600,
    LongPressMs = 3000
};

#ifdef ARDUINO
#include "Arduino.h"
#endif

#endif // COMMON_H