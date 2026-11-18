#pragma once
// Persistent settings (unit, scale, brightness) and the odometer, stored in NVS.
// NVS keys are unchanged from the original sketch, so existing devices keep
// their mileage and settings after flashing.
#include <stdint.h>

struct Unit {
    const char *name;
    float fromKmh;        // km/h -> this unit
    float metersPerUnit;  // metres per distance unit (km / mile / nm)
};

// Dial ticks read 0, step, 2*step ... 6*step; the title shows "x<mult>".
struct ScaleOpt {
    uint8_t step;
    uint8_t mult;
};

extern const Unit     UNITS[];
extern const int      UNIT_COUNT;
extern const ScaleOpt SCALES[];
extern const int      SCALE_COUNT;

inline int scale_max(const ScaleOpt &s) { return 6 * s.step * s.mult; }

void state_begin();                    // load everything from NVS

int             state_unit_index();
const Unit     &state_unit();
void            state_set_unit(int idx);

int             state_scale_index();
const ScaleOpt &state_scale();
void            state_set_scale(int idx);

uint8_t state_brightness();            // 0..255
void    state_set_brightness(uint8_t value, bool persist);
uint8_t brightness_to_pct(uint8_t value);
uint8_t pct_to_brightness(uint8_t pct);

// Odometer: saved each time a whole distance unit is crossed, and on stop.
void     odo_add_meters(float m);
uint32_t odo_meters();
void     odo_flush();
void     odo_reset();
