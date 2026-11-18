#pragma once
// ============================================================================
//  User configuration: everything you are likely to tweak lives here.
// ============================================================================
#include <stdint.h>

// ---------------- GPS wiring (CYD connector CN1) ----------------
// GPIO 6-11 are flash pins on the classic ESP32 and cannot be used.
#define GPS_RX_PIN      22      // NEO-7M TX  -> ESP32 GPIO22
#define GPS_TX_PIN      27      // NEO-7M RX  -> ESP32 GPIO27
#define GPS_BAUD        9600    // NEO-7M factory default

// ---------------- Local time ----------------
#define TZ_OFFSET_MIN   360     // offset from UTC in minutes (UTC+6 = 360)

// ---------------- Diagnostics ----------------
// 1 = print a GPS status line to Serial (115200) every second.
#define GPS_DEBUG       0

// ---------------- Speed processing ----------------
// Hysteresis: must exceed MOVE_ON to count as moving, must fall below
// MOVE_OFF to count as stopped. Prevents 0 <-> speed flicker at low speed.
constexpr float    MOVE_ON_KMH     = 3.0f;
constexpr float    MOVE_OFF_KMH    = 1.5f;

// How long a speed reading stays valid without a new GPS fix.
// NEO-7M updates at 1 Hz, so 3000 ms tolerates ~2 missed sentences.
constexpr uint32_t GPS_STALE_MS    = 3000;

// Exponential smoothing factor applied every UI tick (0..1).
// Higher = more responsive, lower = steadier. 0.3 @ 200 ms ~ 0.7 s lag.
constexpr float    SPEED_SMOOTHING = 0.3f;

// ---------------- UI ----------------
constexpr uint32_t UI_UPDATE_MS     = 200;   // dashboard refresh period
constexpr uint32_t GAUGE_ANIM_MS    = 500;   // needle/arc animation time
constexpr uint16_t LONG_PRESS_MS    = 700;   // hold time to open settings
constexpr uint8_t  BRIGHTNESS_MIN_PCT = 5;   // never allow a black screen

// ---------------- Display ----------------
constexpr uint16_t SCR_W          = 320;     // landscape (rotation 1)
constexpr uint16_t SCR_H          = 240;
constexpr uint16_t DRAW_BUF_LINES = 40;      // LVGL partial buffer height
