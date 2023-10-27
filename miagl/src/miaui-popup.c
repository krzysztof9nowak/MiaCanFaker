#include <miaui-popup.h>

#include <miagl-bitmap.h>
#include <miagl-font.h>
#include <miagl-gfx.h>
#include <miaui-resources.h>

#define MUI_POPUP_ANIMATION_NO_POPUP 0
#define MUI_POPUP_ANIMATION_FADE_IN 1
#define MUI_POPUP_ANIMATION_COUNTDOWN 2
#define MUI_POPUP_ANIMATION_FADE_OUT 3
#define MUI_POPUP_TARGET_HEIGHT 40

typedef struct mui_popup_def {
    const uint32_t* bitmap;
    uint8_t second_line;
    const char text[];
} mui_popup_def_t, *mui_popup_def_ptr;

static mui_popup_def_t PBRAKE_POPUP = {
    .bitmap = IMG_LIGHT_PBRAKE,
    .second_line = 8,
    .text = "Zwolnij\0r\x13""czny!"
};

static mui_popup_def_ptr POPUPS[] = {
    NULL,
    &PBRAKE_POPUP,
};

void mui_PopupUpdate(mui_state_ptr instance, uint32_t delta_ms)
{
    switch (instance->__internal.popup_animation_stage) {
    case MUI_POPUP_ANIMATION_NO_POPUP: 
        return;
    case MUI_POPUP_ANIMATION_FADE_IN:
        instance->__internal.popup_height += 3;
        if (instance->__internal.popup_height > MUI_POPUP_TARGET_HEIGHT) {
            instance->__internal.popup_height = MUI_POPUP_TARGET_HEIGHT;
            instance->__internal.popup_animation_stage = MUI_POPUP_ANIMATION_COUNTDOWN;
        }
        return;
    case MUI_POPUP_ANIMATION_COUNTDOWN:
        instance->__internal.popup_time_left_ms -= delta_ms;
        if (instance->__internal.popup_time_left_ms < 0) {
            instance->__internal.popup_animation_stage = MUI_POPUP_ANIMATION_FADE_OUT;
        }
        return;
    case MUI_POPUP_ANIMATION_FADE_OUT:
        instance->__internal.popup_height -= 3;
        if (instance->__internal.popup_height > MUI_POPUP_TARGET_HEIGHT) {
            instance->__internal.popup_animation_stage = MUI_POPUP_ANIMATION_NO_POPUP;
            instance->__internal.selected_popup = 0;
        }
        return;
    }
}

void mui_PopupDraw(mui_state_ptr instance, miagl_ptr gl)
{
    if (!instance->__internal.selected_popup)
        return;

    uint16_t y = (63 - instance->__internal.popup_height) / 2;
    uint16_t y2 = y + instance->__internal.popup_height + 1;

    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_StrokeRect(gl, 93, y, 161, y2);
    mgl_SetColor(gl, 10);
    mgl_DrawHLine(gl, 94, 162, y2 + 1);
    mgl_DrawVLine(gl, 162, y + 1, y2);
    mgl_SetColor(gl, MIAGL_COLOR_BLACK);
    mgl_FillRect(gl, 94, y + 1, 160, y2 - 1);

    if (instance->__internal.popup_animation_stage == MUI_POPUP_ANIMATION_COUNTDOWN)
    {
        mui_popup_def_ptr current_popup = POPUPS[instance->__internal.selected_popup];
        if (!current_popup) return;
        uint16_t bitmap_x = 128 - mgl_GetBitmapWidth(current_popup->bitmap) / 2;
        mgl_DrawBitmap(gl, bitmap_x, y + 2, current_popup->bitmap);
        mgl_SetColor(gl, MIAGL_COLOR_WHITE);
        mgl_DrawTextCenter(gl, current_popup->text, FNT_HELVETICA_BOLD8, 94, y + 18, 70);
        mgl_DrawTextCenter(gl, current_popup->text + current_popup->second_line, FNT_HELVETICA_BOLD8, 94, y + 29, 70);
    }
}

void mui_ShowPopup(mui_state_ptr instance, mui_popup_type_t type, int32_t duration_ms)
{
    instance->__internal.selected_popup = type;
    instance->__internal.popup_time_left_ms = duration_ms;

    switch (instance->__internal.popup_animation_stage) {
        case MUI_POPUP_ANIMATION_NO_POPUP: 
            instance->__internal.popup_height = 0;
            instance->__internal.popup_animation_stage = MUI_POPUP_ANIMATION_FADE_IN;
            break;
        case MUI_POPUP_ANIMATION_FADE_OUT:
            instance->__internal.popup_animation_stage = MUI_POPUP_ANIMATION_FADE_IN;
            instance->__internal.popup_height = 0;
            break;
    }
}

void mui_HidePopup(mui_state_ptr instance)
{
    instance->__internal.popup_time_left_ms = -1;
}
