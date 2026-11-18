// ============================================================================
//  GPS Speedometer: ESP32-2432S028R ("Cheap Yellow Display") + u-blox NEO-7M
//
//  Long-press the gauge to open Settings (scale, unit, brightness, clear odo).
//  All tunables live in config.h.
// ============================================================================
#include <Arduino.h>
#include <lvgl.h>

#include "app_state.h"
#include "config.h"
#include "dashboard.h"
#include "display.h"
#include "gps.h"
#include "settings_ui.h"
#include "ui.h"

void setup()
{
    Serial.begin(115200);

    gps_begin();
    state_begin();
    display_begin(state_brightness());

    ui_init();             // SquareLine-generated gauge screen
    dashboard_begin();
    settings_ui_begin();
}

void loop()
{
    gps_poll();

    static uint32_t lastUi = 0;
    if (millis() - lastUi >= UI_UPDATE_MS) {
        lastUi = millis();
        dashboard_update();
    }

    gps_debug_print();
    lv_timer_handler();
    delay(5);
}
