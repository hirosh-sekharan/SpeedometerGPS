#pragma once
// Main gauge screen (ui_Screen1 from SquareLine): needle, arc and labels.

void dashboard_begin();        // call after ui_init()
void dashboard_apply_scale();  // re-label dial after a unit/scale change
void dashboard_update();       // call every UI_UPDATE_MS
