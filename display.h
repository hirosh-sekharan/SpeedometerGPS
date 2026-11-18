#pragma once
// LovyanGFX panel + touch glue for LVGL 8.
#include <stdint.h>

void display_begin(uint8_t brightness);   // inits panel, LVGL, display + touch drivers
void display_set_brightness(uint8_t value);
