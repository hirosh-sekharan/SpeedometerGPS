#include "settings_ui.h"

#include <lvgl.h>

#include <cstdio>

#include "app_state.h"
#include "config.h"
#include "dashboard.h"
#include "display.h"
#include "ui.h"

static lv_obj_t *scrSettings, *scrScale;
static lv_obj_t *lblScaleVal, *lblUnitVal, *lblBriVal;
static lv_obj_t *scaleBtn[8], *scaleLbl[8];   // >= SCALE_COUNT

// ---------------- Styles ("glass" look) ----------------
static lv_style_t stGlass, stGlassPr, stGlassChk, stDialog;
static lv_style_t stSlMain, stSlInd, stSlKnob;

static void init_styles()
{
    lv_style_init(&stGlass);
    lv_style_set_bg_color(&stGlass, lv_color_hex(0x0A0F18));
    lv_style_set_bg_opa(&stGlass, 150);
    lv_style_set_bg_grad_dir(&stGlass, LV_GRAD_DIR_NONE);
    lv_style_set_border_color(&stGlass, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&stGlass, 60);
    lv_style_set_border_width(&stGlass, 1);
    lv_style_set_radius(&stGlass, 12);
    lv_style_set_shadow_width(&stGlass, 0);
    lv_style_set_text_color(&stGlass, lv_color_hex(0xFFFFFF));

    lv_style_init(&stGlassPr);
    lv_style_set_bg_color(&stGlassPr, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&stGlassPr, 60);
    lv_style_set_color_filter_opa(&stGlassPr, 0);

    lv_style_init(&stGlassChk);
    lv_style_set_bg_color(&stGlassChk, lv_color_hex(0x2F80ED));
    lv_style_set_bg_opa(&stGlassChk, 150);
    lv_style_set_border_color(&stGlassChk, lv_color_hex(0x7DB4FF));
    lv_style_set_border_opa(&stGlassChk, 200);

    lv_style_init(&stDialog);
    lv_style_set_bg_color(&stDialog, lv_color_hex(0x0A0F18));
    lv_style_set_bg_opa(&stDialog, 235);
    lv_style_set_border_color(&stDialog, lv_color_hex(0xFFFFFF));
    lv_style_set_border_opa(&stDialog, 80);
    lv_style_set_border_width(&stDialog, 1);
    lv_style_set_radius(&stDialog, 14);
    lv_style_set_shadow_width(&stDialog, 0);
    lv_style_set_text_color(&stDialog, lv_color_hex(0xFFFFFF));
    lv_style_set_text_align(&stDialog, LV_TEXT_ALIGN_CENTER);

    lv_style_init(&stSlMain);
    lv_style_set_bg_color(&stSlMain, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&stSlMain, 50);
    lv_style_set_radius(&stSlMain, 6);
    lv_style_set_height(&stSlMain, 8);

    lv_style_init(&stSlInd);
    lv_style_set_bg_color(&stSlInd, lv_color_hex(0x4AA3FF));
    lv_style_set_bg_opa(&stSlInd, 255);
    lv_style_set_radius(&stSlInd, 6);

    lv_style_init(&stSlKnob);
    lv_style_set_bg_color(&stSlKnob, lv_color_hex(0xFFFFFF));
    lv_style_set_bg_opa(&stSlKnob, 255);
    lv_style_set_radius(&stSlKnob, LV_RADIUS_CIRCLE);
    lv_style_set_pad_all(&stSlKnob, 5);
}

static void glass(lv_obj_t *o)
{
    lv_obj_add_style(o, &stGlass, 0);
    lv_obj_add_style(o, &stGlassPr, LV_STATE_PRESSED);
    lv_obj_add_style(o, &stGlassChk, LV_STATE_CHECKED);
}

// ---------------- Refresh helpers ----------------
static void format_scale(char *buf, size_t n, int scaleIdx)
{
    snprintf(buf, n, "%d %s", scale_max(SCALES[scaleIdx]), state_unit().name);
}

static void refresh_settings()
{
    char buf[24];
    format_scale(buf, sizeof(buf), state_scale_index());
    lv_label_set_text(lblScaleVal, buf);
    lv_label_set_text(lblUnitVal, state_unit().name);
}

static void refresh_scale_screen()
{
    char buf[24];
    for (int i = 0; i < SCALE_COUNT; i++) {
        format_scale(buf, sizeof(buf), i);
        lv_label_set_text(scaleLbl[i], buf);
        if (i == state_scale_index()) lv_obj_add_state(scaleBtn[i], LV_STATE_CHECKED);
        else                          lv_obj_clear_state(scaleBtn[i], LV_STATE_CHECKED);
    }
}

// ---------------- Builders ----------------
static lv_obj_t *make_screen(const char *title, lv_event_cb_t backCb)
{
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x22344D), 0);
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x04070C), 0);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_opa(scr, 255, 0);

    lv_obj_t *back = lv_btn_create(scr);
    lv_obj_set_size(back, 56, 30);
    lv_obj_align(back, LV_ALIGN_TOP_LEFT, 4, 3);
    glass(back);
    lv_obj_t *bl = lv_label_create(back);
    lv_label_set_text(bl, LV_SYMBOL_LEFT);
    lv_obj_center(bl);
    lv_obj_add_event_cb(back, backCb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *t = lv_label_create(scr);
    lv_label_set_text(t, title);
    lv_obj_set_style_text_color(t, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(t, LV_ALIGN_TOP_MID, 0, 10);
    return scr;
}

static lv_obj_t *make_row(lv_obj_t *scr, int y, const char *text)
{
    lv_obj_t *row = lv_btn_create(scr);
    lv_obj_set_size(row, 312, 42);
    lv_obj_set_pos(row, 4, y);
    glass(row);
    lv_obj_t *l = lv_label_create(row);
    lv_label_set_text(l, text);
    lv_obj_align(l, LV_ALIGN_LEFT_MID, 4, 0);
    return row;
}

static lv_obj_t *make_value(lv_obj_t *row)
{
    lv_obj_t *v = lv_label_create(row);
    lv_label_set_text(v, "");
    lv_obj_align(v, LV_ALIGN_RIGHT_MID, -4, 0);
    return v;
}

// ---------------- Event handlers ----------------
static void on_gauge_long_press(lv_event_t *)
{
    lv_indev_wait_release(lv_indev_get_act());   // don't let the release click a row
    refresh_settings();
    lv_scr_load(scrSettings);
}

static void on_settings_back(lv_event_t *) { lv_scr_load(ui_Screen1); }
static void on_scale_back(lv_event_t *)    { lv_scr_load(scrSettings); }

static void on_scale_row(lv_event_t *)
{
    refresh_scale_screen();
    lv_scr_load(scrScale);
}

static void on_scale_pick(lv_event_t *e)
{
    state_set_scale((int)(intptr_t)lv_event_get_user_data(e));
    dashboard_apply_scale();
    refresh_settings();
    lv_scr_load(scrSettings);
}

static void on_unit_row(lv_event_t *)
{
    state_set_unit(state_unit_index() + 1);
    dashboard_apply_scale();
    refresh_settings();
}

static void on_brightness(lv_event_t *e)
{
    lv_obj_t *s = lv_event_get_target(e);
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        int pct = lv_slider_get_value(s);
        uint8_t b = pct_to_brightness((uint8_t)pct);
        display_set_brightness(b);
        state_set_brightness(b, false);   // live preview, no flash write
        char buf[8];
        snprintf(buf, sizeof(buf), "%d%%", pct);
        lv_label_set_text(lblBriVal, buf);
    } else {
        state_set_brightness(state_brightness(), true);   // persist on release
    }
}

static void on_clear_confirm(lv_event_t *e)
{
    lv_obj_t *mb = lv_event_get_current_target(e);
    if (lv_msgbox_get_active_btn(mb) == 0) odo_reset();   // "Clear"
    lv_msgbox_close(mb);
}

static void on_clear_row(lv_event_t *)
{
    static const char *btns[] = {"Clear", "Cancel", ""};
    lv_obj_t *mb = lv_msgbox_create(NULL, "Clear mileage", "Reset total distance to 0?", btns, false);
    lv_obj_add_style(mb, &stDialog, 0);
    lv_obj_set_size(mb, 260, LV_SIZE_CONTENT);
    lv_obj_set_style_text_align(lv_msgbox_get_title(mb), LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_align(lv_msgbox_get_text(mb), LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *bm = lv_msgbox_get_btns(mb);
    lv_obj_set_style_translate_x(bm, 30, 0);   // visually centre the button row
    lv_obj_add_style(bm, &stGlass, LV_PART_ITEMS);
    lv_obj_add_style(bm, &stGlassPr, LV_PART_ITEMS | LV_STATE_PRESSED);

    lv_obj_add_event_cb(mb, on_clear_confirm, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_update_layout(mb);
    lv_obj_align(mb, LV_ALIGN_CENTER, 0, 0);
}

// ---------------- Public ----------------
void settings_ui_begin()
{
    init_styles();

    // Main settings screen
    scrSettings = make_screen("Settings & Customization", on_settings_back);

    lv_obj_t *r = make_row(scrSettings, 40, "Scale");
    lblScaleVal = make_value(r);
    lv_obj_add_event_cb(r, on_scale_row, LV_EVENT_CLICKED, NULL);

    r = make_row(scrSettings, 86, "Unit");
    lblUnitVal = make_value(r);
    lv_obj_add_event_cb(r, on_unit_row, LV_EVENT_CLICKED, NULL);

    r = make_row(scrSettings, 132, "Brightness");
    lv_obj_clear_flag(r, LV_OBJ_FLAG_CLICKABLE);
    int pct = brightness_to_pct(state_brightness());
    if (pct < BRIGHTNESS_MIN_PCT) pct = BRIGHTNESS_MIN_PCT;
    lblBriVal = make_value(r);
    char pb[8];
    snprintf(pb, sizeof(pb), "%d%%", pct);
    lv_label_set_text(lblBriVal, pb);

    lv_obj_t *sl = lv_slider_create(r);
    lv_obj_set_width(sl, 130);
    lv_obj_align(sl, LV_ALIGN_RIGHT_MID, -56, 0);
    lv_obj_set_ext_click_area(sl, 12);
    lv_obj_add_style(sl, &stSlMain, LV_PART_MAIN);
    lv_obj_add_style(sl, &stSlInd, LV_PART_INDICATOR);
    lv_obj_add_style(sl, &stSlKnob, LV_PART_KNOB);
    lv_slider_set_range(sl, BRIGHTNESS_MIN_PCT, 100);
    lv_slider_set_value(sl, pct, LV_ANIM_OFF);
    lv_obj_add_event_cb(sl, on_brightness, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(sl, on_brightness, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(sl, on_brightness, LV_EVENT_PRESS_LOST, NULL);

    r = make_row(scrSettings, 178, "Clear mileage");
    lv_obj_add_event_cb(r, on_clear_row, LV_EVENT_CLICKED, NULL);

    // Scale (max speed) picker: 2 columns x 3 rows
    scrScale = make_screen("Max speed", on_scale_back);
    for (int i = 0; i < SCALE_COUNT && i < (int)(sizeof(scaleBtn) / sizeof(scaleBtn[0])); i++) {
        lv_obj_t *b = lv_btn_create(scrScale);
        lv_obj_set_size(b, 152, 54);
        lv_obj_set_pos(b, 4 + (i % 2) * 160, 40 + (i / 2) * 62);
        glass(b);
        lv_obj_add_flag(b, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_event_cb(b, on_scale_pick, LV_EVENT_CLICKED, (void *)(intptr_t)i);
        scaleBtn[i] = b;
        scaleLbl[i] = lv_label_create(b);
        lv_obj_center(scaleLbl[i]);
    }

    // Long-press anywhere on the gauge opens settings
    lv_obj_add_event_cb(ui_Screen1, on_gauge_long_press, LV_EVENT_LONG_PRESSED, NULL);
}
