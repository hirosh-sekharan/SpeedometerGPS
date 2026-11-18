#include "app_state.h"

#include <Preferences.h>

#include "config.h"

const Unit UNITS[] = {
    {"km/h",  1.0f,      1000.0f},
    {"mph",   0.621371f, 1609.344f},
    {"knots", 0.539957f, 1852.0f},
};
const int UNIT_COUNT = sizeof(UNITS) / sizeof(UNITS[0]);

const ScaleOpt SCALES[] = {
    {2, 1},    // 12
    {5, 1},    // 30
    {1, 10},   // 60
    {2, 10},   // 120
    {3, 10},   // 180
    {4, 10},   // 240
};
const int SCALE_COUNT = sizeof(SCALES) / sizeof(SCALES[0]);

static const char *NS        = "speedo";
static const char *KEY_ODO   = "odoM";
static const char *KEY_UNIT  = "unit";
static const char *KEY_SCALE = "max";
static const char *KEY_BRI   = "bri";

static Preferences prefs;
static int      unitIdx    = 0;
static int      scaleIdx   = 3;
static uint8_t  brightness = 255;

static uint32_t totalM = 0;   // whole metres
static float    accM   = 0;   // sub-metre remainder
static uint32_t savedM = 0;   // value last written to NVS

void state_begin()
{
    prefs.begin(NS, false);
    totalM   = prefs.getULong(KEY_ODO, 0);
    savedM   = totalM;
    unitIdx  = prefs.getUChar(KEY_UNIT, 0) % UNIT_COUNT;
    scaleIdx = prefs.getUChar(KEY_SCALE, 3) % SCALE_COUNT;

    // Guard against a stored 0 (or near-0) which would leave the screen black.
    uint8_t minB = pct_to_brightness(BRIGHTNESS_MIN_PCT);
    brightness   = prefs.getUChar(KEY_BRI, 255);
    if (brightness < minB) brightness = minB;
}

// ---------------- Unit / scale ----------------
int         state_unit_index() { return unitIdx; }
const Unit &state_unit()       { return UNITS[unitIdx]; }

void state_set_unit(int idx)
{
    unitIdx = ((idx % UNIT_COUNT) + UNIT_COUNT) % UNIT_COUNT;
    prefs.putUChar(KEY_UNIT, (uint8_t)unitIdx);
}

int             state_scale_index() { return scaleIdx; }
const ScaleOpt &state_scale()       { return SCALES[scaleIdx]; }

void state_set_scale(int idx)
{
    if (idx < 0 || idx >= SCALE_COUNT) return;
    scaleIdx = idx;
    prefs.putUChar(KEY_SCALE, (uint8_t)scaleIdx);
}

// ---------------- Brightness ----------------
uint8_t state_brightness() { return brightness; }

void state_set_brightness(uint8_t value, bool persist)
{
    brightness = value;
    if (persist) prefs.putUChar(KEY_BRI, brightness);
}

uint8_t brightness_to_pct(uint8_t v) { return (uint8_t)((v * 100 + 127) / 255); }
uint8_t pct_to_brightness(uint8_t p) { return (uint8_t)((p * 255 + 50) / 100); }

// ---------------- Odometer ----------------
void odo_add_meters(float m)
{
    if (m <= 0) return;
    accM += m;
    uint32_t whole = (uint32_t)accM;
    if (!whole) return;
    totalM += whole;
    accM   -= whole;

    // Persist once per whole distance unit to limit flash wear.
    float per = UNITS[unitIdx].metersPerUnit;
    if ((uint32_t)(totalM / per) != (uint32_t)(savedM / per)) odo_flush();
}

uint32_t odo_meters() { return totalM; }

void odo_flush()
{
    if (totalM == savedM) return;
    prefs.putULong(KEY_ODO, totalM);
    savedM = totalM;
}

void odo_reset()
{
    totalM = 0;
    accM   = 0;
    savedM = 0;
    prefs.putULong(KEY_ODO, 0);
}
