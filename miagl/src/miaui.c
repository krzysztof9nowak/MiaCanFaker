#include <miaui.h>

#include <miagl-bitmap.h>
#include <miagl-gfx.h>
#include <miaui-gauge.h>
#include <miaui-popup.h>
#include <miaui-resources.h>
#include <miaui-utils.h>


#define MUI_DISBALED_LIGHT_COLOR 4
#define MUI_ENABLED_LIGHT_COLOR MIAGL_COLOR_WHITE
#define MUI_LIGHT_COLOR(enabled) (enabled) ? (MUI_ENABLED_LIGHT_COLOR) : (MUI_DISBALED_LIGHT_COLOR)

static const uint16_t SPEEDO_HAND_LUT[] = {96, 43, 96, 47, 83, 45, 96, 42, 96, 46, 83, 44, 96, 41,
96, 45, 83, 42, 96, 40, 96, 44, 83, 41, 96, 39, 96, 43, 83, 39, 96, 38, 96, 42, 83, 38, 96, 37, 95,
41, 83, 36, 96, 36, 95, 40, 83, 35, 96, 35, 95, 38, 83, 33, 96, 33, 95, 37, 83, 32, 96, 32, 95, 36,
83, 30, 96, 31, 95, 35, 83, 29, 96, 30, 94, 34, 83, 27, 96, 29, 94, 33, 83, 26, 96, 28, 94, 32, 83,
24, 96, 27, 94, 30, 83, 23, 95, 26, 94, 29, 83, 22, 95, 25, 93, 28, 83, 20, 95, 23, 93, 27, 83, 19,
95, 22, 93, 26, 83, 17, 95, 21, 93, 24, 83, 16, 95, 20, 93, 23, 83, 14, 95, 19, 92, 22, 83, 13, 95,
17, 92, 21, 83, 11, 95, 16, 92, 19, 83, 10, 94, 15, 92, 18, 83, 8, 94, 14, 92, 17, 83, 7, 94, 12,
92, 15, 83, 5, 94, 11, 91, 14, 83, 4, 94, 10, 91, 13, 83, 2, 94, 9, 91, 11, 83, 1, 95, 9, 92, 12,
84, 1, 96, 9, 94, 12, 86, 1, 98, 9, 95, 12, 87, 1, 99, 9, 96, 12, 89, 1, 100, 10, 97, 12, 90, 1,
102, 10, 99, 12, 92, 1, 103, 10, 100, 12, 93, 1, 104, 10, 101, 13, 95, 1, 105, 10, 102, 13, 96, 1,
107, 11, 103, 13, 98, 1, 108, 11, 105, 13, 99, 1, 109, 11, 106, 13, 101, 1, 110, 11, 107, 13, 102,
1, 112, 11, 108, 13, 104, 1, 113, 12, 109, 13, 105, 1, 114, 12, 110, 14, 106, 1, 115, 12, 111, 14,
108, 1, 116, 12, 113, 14, 109, 1, 117, 12, 114, 14, 111, 1, 118, 13, 115, 14, 112, 1, 120, 13, 116,
14, 114, 1, 121, 13, 117, 14, 115, 1, 122, 13, 118, 14, 117, 1, 123, 13, 119, 14, 118, 1, 124, 13,
120, 14, 120, 1, 125, 14, 121, 14, 121, 1, 126, 14, 122, 14, 123, 1, 127, 14, 123, 14, 124, 1, 128,
14, 124, 14, 126, 1, 129, 14, 125, 14, 127, 1, 130, 14, 126, 14, 128, 1, 131, 14, 127, 14, 130, 1,
132, 14, 128, 14, 131, 1, 133, 14, 129, 14, 133, 1, 134, 14, 130, 13, 134, 1, 135, 14, 131, 13,
136, 1, 136, 14, 132, 13, 137, 1, 137, 14, 133, 13, 139, 1, 138, 14, 134, 13, 140, 1, 139, 14, 136,
13, 142, 1, 140, 14, 137, 12, 143, 1, 141, 14, 138, 12, 145, 1, 143, 14, 139, 12, 146, 1, 144, 14,
140, 12, 148, 1, 145, 13, 141, 12, 149, 1, 146, 13, 142, 11, 150, 1, 147, 13, 144, 11, 152, 1, 148,
13, 145, 11, 153, 1, 149, 13, 146, 11, 155, 1, 151, 13, 147, 11, 156, 1, 152, 13, 149, 10, 158, 1,
153, 13, 150, 10, 159, 1, 154, 12, 151, 10, 161, 1, 155, 12, 152, 10, 162, 1, 157, 12, 154, 10,
164, 1, 158, 12, 155, 9, 165, 1, 159, 12, 156, 9, 167, 1, 160, 12, 158, 9, 168, 1, 162, 12, 159, 9,
170, 1, 163, 11, 160, 9, 171, 1, 163, 13, 160, 10, 171, 2, 163, 14, 160, 11, 171, 4, 162, 15, 160,
12, 171, 5, 162, 17, 160, 14, 171, 7, 162, 18, 160, 15, 171, 8, 162, 19, 159, 16, 171, 10, 162, 21,
159, 17, 171, 11, 162, 22, 159, 19, 171, 13, 161, 23, 159, 20, 171, 14, 161, 24, 159, 21, 171, 16,
161, 26, 159, 22, 171, 17, 161, 27, 159, 23, 171, 19, 161, 28, 159, 25, 171, 20, 160, 29, 159, 26,
171, 22, 160, 30, 158, 27, 171, 23, 160, 32, 158, 28, 171, 24, 160, 33, 158, 29, 171, 26, 160, 34,
158, 30, 171, 27, 159, 35, 158, 31, 171, 29, 159, 36, 158, 32, 171, 30, 159, 37, 158, 33, 171, 32,
159, 38, 158, 35, 171, 33, 159, 40, 158, 36, 171, 35, 159, 41, 158, 37, 171, 36, 158, 42, 158, 38,
171, 38, 158, 43, 158, 39, 171, 39, 158, 44, 158, 40, 171, 41, 158, 45, 158, 41, 171, 42, 158, 46,
158, 42, 171, 44, 158, 47, 158, 43, 171, 45};

static const mui_gauge_def_t VOLTS_GAUGE = {
    .x = 23,
    .y = 10,
    .width = 3,
    .height = 50,
    .center_pt = 60,
    .min = 700,
    .max = 920,
    .reverse = true,
    .colors = {13, 12, 11},
};

static const mui_gauge_def_t AMPS_GAUGE = {
    .x = 33,
    .y = 10,
    .width = 3,
    .height = 50,
    .center_pt = 34,
    .min = -400,
    .max = 400,
    .reverse = true,
    .colors = {13, 12, 11},
};

static const mui_gauge_def_t BATTERY_GAUGE = {
    .x = 176,
    .y = 2,
    .width = 58,
    .height = 5,
    .center_pt = 175,
    .min = 0,
    .max = 100,
    .reverse = false,
    .colors = {9, 10, 11, 12, 13},
};

void mui_InitInternalState(mui_state_ptr instance)
{
    instance->__internal.elapsed_time_ms = 0;
}

void mui_Update(mui_state_ptr instance, uint32_t delta_ms)
{
    instance->__internal.elapsed_time_ms += delta_ms;
    mui_PopupUpdate(instance, delta_ms);
}

void mui_Draw(mui_state_ptr instance, miagl_ptr gl)
{
    mgl_SetBackgroundBitmap(gl, BG_BACKGROUND, 256 * 64 / 2);
    
    static char buffer[64];

    // Draw cell voltage
    mgl_SetColor(gl, 4);
    mgl_FillRect(gl, 0, 0, 19, 8);
    mgl_DrawVLine(gl, 20, 2, 6);
    uint32_t cell_voltage = instance->capacitor_voltage * 10 / instance->cell_count;
    mui_UFixedPointToStr(buffer, cell_voltage, 2);
    mgl_SetColor(gl, 11);
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 1, 1, 19);

    // Draw gauge shadows
    mgl_SetColor(gl, 8);
    mgl_DrawVLine(gl, 27, 10, 60);
    mgl_DrawHLine(gl, 23, 27, 61);
    mgl_DrawVLine(gl, 37, 10, 60);
    mgl_DrawHLine(gl, 33, 37, 61);

    // Draw volts scale
    uint16_t original_volts_coord = mui_CalculateGaugeCoord(VOLTS_GAUGE.y, VOLTS_GAUGE.height, true, 
                                                   VOLTS_GAUGE.min, VOLTS_GAUGE.max, instance->capacitor_voltage);
    uint16_t volts_coord = original_volts_coord;
    if (volts_coord < 14) volts_coord = 14;
    if (volts_coord > 57) volts_coord = 57;
    mgl_DrawBitmap(gl, 0, volts_coord - 4, IMG_SCALE_VOLTS);
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_DrawHLine(gl, 18, 23, original_volts_coord);
    mui_UFixedPointToStr(buffer, instance->capacitor_voltage, 1);
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 1, volts_coord - 3, 19);

    // Draw amps scale
    uint16_t original_amps_coord = mui_CalculateGaugeCoord(AMPS_GAUGE.y, AMPS_GAUGE.height, true, 
                                                   AMPS_GAUGE.min, AMPS_GAUGE.max, instance->motor_current);
    uint16_t amps_coord = original_amps_coord;
    if (amps_coord > 57) amps_coord = 57;
    mgl_DrawBitmap(gl, 39, amps_coord - 4, IMG_SCALE_VOLTS);
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_DrawHLine(gl, 35, 39, original_amps_coord);
    mui_FixedPointToStr(buffer, instance->motor_current, 0);
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 40, amps_coord - 3, 20);

    // Draw electricity gauges
    mui_DrawVGauge(gl, &VOLTS_GAUGE, instance->capacitor_voltage);
    mui_DrawVGauge(gl, &AMPS_GAUGE, instance->motor_current);

    // Draw gear selection
    static const uint32_t* GEAR_BITMAPS[] = { IMG_GEARBOX_D, IMG_GEARBOX_N, IMG_GEARBOX_R };
    mgl_DrawBitmap(gl, 62, 0, GEAR_BITMAPS[instance->gear]);

    // Draw speedometer hand
    uint16_t truncated_speed = instance->vehicle_speed;
    if (truncated_speed > 120) truncated_speed = 120;
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_FillTriangleAA(gl, 
        SPEEDO_HAND_LUT[truncated_speed * 6 + 0], 
        SPEEDO_HAND_LUT[truncated_speed * 6 + 1], 
        SPEEDO_HAND_LUT[truncated_speed * 6 + 2], 
        SPEEDO_HAND_LUT[truncated_speed * 6 + 3], 
        SPEEDO_HAND_LUT[truncated_speed * 6 + 4], 
        SPEEDO_HAND_LUT[truncated_speed * 6 + 5]);

    // Draw speedometer digits
    if (instance->vehicle_speed >= 100) {
        mgl_SetExtraSpacing(gl, -1);
    }
    mui_UFixedPointToStr(buffer, instance->vehicle_speed, 0);
    mgl_DrawTextCenter(gl, buffer, FNT_SPEEDO, 93, 16, 70);
    mgl_ClearExtraSpacing(gl);

    // Draw additional control lights
    mgl_SetColor(gl, MUI_LIGHT_COLOR(instance->light_status & MUI_STATUS_LEFT_BLINKER_ON));
    mgl_DrawTintedBitmap(gl, 82, 51, IMG_LIGHT_BLINKER_L);

    mgl_SetColor(gl, MUI_LIGHT_COLOR(instance->light_status & MUI_STATUS_LEFT_DOOR_OPEN));
    mgl_DrawTintedBitmap(gl, 100, 51, IMG_LIGHT_DOOR_L);

    mgl_SetColor(gl, MUI_LIGHT_COLOR(instance->light_status & MUI_STATUS_MOTOR_FAN_RUNNING));
    mgl_DrawTintedBitmap(gl, 118, 51, IMG_LIGHT_MOTOR_FAN);

    mgl_SetColor(gl, MUI_LIGHT_COLOR(instance->light_status & MUI_STATUS_RIGHT_DOOR_OPEN));
    mgl_DrawTintedBitmap(gl, 137, 51, IMG_LIGHT_DOOR_R);

    mgl_SetColor(gl, MUI_LIGHT_COLOR(instance->light_status & MUI_STATUS_RIGHT_BLINKER_ON));
    mgl_DrawTintedBitmap(gl, 155, 51, IMG_LIGHT_BLINKER_R);

    // Draw battery gauge
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mui_DrawHGauge(gl, &BATTERY_GAUGE, instance->battery_level);
    mgl_DrawVLine(gl, 235, 3, 5);
    mgl_DrawXorBitmap(gl, 203, 2, IMG_BATTERY_SYMBOL);
    mui_UFixedPointToStr(buffer, instance->battery_level, 0);
    mgl_DrawTextRight(gl, buffer, FNT_DIGITS_4X7, 237, 1, 13);

    // Draw estimated range
    mgl_SetColor(gl, 12);
    if (instance->estimated_range != 0xFFFF) {
        mui_UFixedPointToStr(buffer, instance->estimated_range, 0);
    } else {
        buffer[0] = buffer[1] = buffer[2] = '-';
        buffer[3] = 0;
    }
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 202, 11, 15);

    // Draw motor temp
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mui_FixedPointToStr(buffer, instance->motor_temp, 0);
    mgl_DrawTextRight(gl, buffer, FNT_DIGITS, 191, 32, 16);

    // Draw inverter temp
    mui_FixedPointToStr(buffer, instance->inverter_temp, 0);
    mgl_DrawTextRight(gl, buffer, FNT_DIGITS, 232, 32, 16);

    // Draw trip meter
    mui_UFixedPointToStr(buffer, instance->trip_meter, 2);
    mgl_DrawTextRight(gl, buffer, FNT_DIGITS, 213, 43, 32);

    // Draw odometer
    mui_UFixedPointToStr(buffer, (instance->odometer > 999999) ? 999999 : instance->odometer, 0);
    mgl_DrawTextRight(gl, buffer, FNT_DIGITS, 208, 54, 48);

    // Draw popup
    mui_PopupDraw(instance, gl);

    // Draw debug
    if (instance->debug_text) {
        mgl_SetColor(gl, MIAGL_COLOR_BLACK);
        mgl_FillRect(gl, 0, 0, 255, 13);
        mgl_SetColor(gl, MIAGL_COLOR_WHITE);
        mgl_DrawText(gl, instance->debug_text, FNT_HELVETICA_BOLD8, 0, 2);
    }
}
