#pragma once
// GPS input: owns the UART + NMEA parser and turns raw speed into a
// stable, filtered value with a moving/stopped decision.
#include <stdint.h>

struct GpsSample {
    float   kmh;          // filtered speed, 0 when not moving
    bool    moving;       // hysteresis-based moving state
    bool    courseValid;  // course is recent and we are moving
    float   courseDeg;
    uint8_t sats;         // 0 if unknown
    bool    timeValid;
    int     localMinutes; // minutes since local midnight (0..1439)
};

void      gps_begin();
void      gps_poll();        // feed UART bytes to the parser; call every loop()
GpsSample gps_sample();      // call once per UI tick (filter is rate-dependent)
void      gps_debug_print(); // prints a status line once per second if GPS_DEBUG
