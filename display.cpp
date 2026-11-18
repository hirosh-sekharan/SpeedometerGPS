#include "display.h"

#include <lvgl.h>

#include "LGFX_CYD.hpp"
#include "config.h"

static LGFX tft;

static lv_disp_draw_buf_t drawBuf;
static lv_color_t         buf1[SCR_W * DRAW_BUF_LINES];

static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    tft.startWrite();
    tft.pushImage(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);
    tft.endWrite();
    lv_disp_flush_ready(drv);
}

static void touch_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    uint16_t x, y;
    if (tft.getTouch(&x, &y)) {
        data->state   = LV_INDEV_STATE_PR;
        data->point.x = x;
        data->point.y = y;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

void display_begin(uint8_t brightness)
{
    tft.init();
    tft.setRotation(1);
    tft.setBrightness(brightness);

    lv_init();
    lv_disp_draw_buf_init(&drawBuf, buf1, NULL, SCR_W * DRAW_BUF_LINES);

    static lv_disp_drv_t dispDrv;
    lv_disp_drv_init(&dispDrv);
    dispDrv.hor_res  = SCR_W;
    dispDrv.ver_res  = SCR_H;
    dispDrv.flush_cb = disp_flush;
    dispDrv.draw_buf = &drawBuf;
    lv_disp_drv_register(&dispDrv);

    static lv_indev_drv_t indevDrv;
    lv_indev_drv_init(&indevDrv);
    indevDrv.type            = LV_INDEV_TYPE_POINTER;
    indevDrv.read_cb         = touch_read;
    indevDrv.long_press_time = LONG_PRESS_MS;
    lv_indev_drv_register(&indevDrv);
}

void display_set_brightness(uint8_t value)
{
    tft.setBrightness(value);
}
