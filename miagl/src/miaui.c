#include <miaui.h>

#include <miagl-bitmap.h>
#include <miagl-gfx.h>
#include <miaui-gauge.h>
#include <miaui-resources.h>
#include <miaui-utils.h>


static const uint16_t SPEEDO_HAND_LUT[] = {96, 44, 96, 48, 83, 46, 96, 43, 96, 47, 83, 44, 96, 42,
96, 46, 83, 43, 96, 41, 96, 45, 83, 42, 96, 40, 96, 44, 83, 40, 96, 39, 96, 43, 83, 38, 96, 37, 95,
41, 83, 37, 96, 36, 95, 40, 83, 36, 96, 35, 95, 39, 83, 34, 96, 34, 95, 38, 83, 32, 96, 33, 95, 37,
83, 31, 96, 32, 95, 36, 83, 30, 96, 31, 94, 35, 83, 28, 96, 30, 94, 33, 83, 26, 96, 29, 94, 32, 83,
25, 96, 27, 94, 31, 83, 24, 95, 26, 94, 30, 83, 22, 95, 25, 93, 28, 83, 20, 95, 24, 93, 27, 83, 19,
95, 23, 93, 26, 83, 18, 95, 21, 93, 25, 83, 16, 95, 20, 93, 23, 83, 14, 95, 19, 92, 22, 83, 13, 95,
18, 92, 21, 83, 12, 95, 16, 92, 20, 83, 10, 94, 15, 92, 18, 83, 8, 94, 14, 92, 17, 83, 7, 94, 13,
92, 16, 83, 6, 94, 11, 91, 14, 83, 4, 94, 10, 91, 13, 83, 2, 94, 9, 91, 11, 83, 1, 95, 9, 92, 12,
84, 1, 96, 9, 94, 12, 86, 1, 98, 9, 95, 12, 87, 1, 99, 9, 96, 12, 89, 1, 100, 10, 97, 12, 90, 1,
102, 10, 99, 12, 92, 1, 103, 10, 100, 12, 93, 1, 104, 10, 101, 13, 95, 1, 106, 10, 102, 13, 96, 1,
107, 11, 103, 13, 98, 1, 108, 11, 105, 13, 99, 1, 109, 11, 106, 13, 101, 1, 110, 11, 107, 13, 102,
1, 112, 12, 108, 13, 104, 1, 113, 12, 109, 14, 105, 1, 114, 12, 110, 14, 107, 1, 115, 12, 111, 14,
108, 1, 116, 12, 113, 14, 110, 1, 117, 13, 114, 14, 111, 1, 119, 13, 115, 14, 113, 1, 120, 13, 116,
14, 114, 1, 121, 13, 117, 14, 116, 1, 122, 13, 118, 14, 117, 1, 123, 13, 119, 14, 119, 1, 124, 14,
120, 14, 120, 1, 125, 14, 121, 14, 122, 1, 126, 14, 122, 14, 123, 1, 127, 14, 123, 14, 125, 1, 128,
14, 124, 14, 126, 1, 129, 14, 125, 14, 128, 1, 130, 14, 126, 14, 129, 1, 131, 14, 127, 14, 130, 1,
132, 14, 128, 14, 132, 1, 133, 14, 129, 13, 133, 1, 134, 14, 130, 13, 135, 1, 135, 14, 131, 13,
136, 1, 136, 14, 132, 13, 138, 1, 137, 14, 133, 13, 139, 1, 138, 14, 135, 13, 141, 1, 139, 14, 136,
12, 142, 1, 140, 14, 137, 12, 144, 1, 142, 14, 138, 12, 145, 1, 143, 14, 139, 12, 147, 1, 144, 13,
140, 11, 148, 1, 145, 13, 142, 11, 150, 1, 146, 13, 143, 11, 151, 1, 147, 13, 144, 11, 153, 1, 149,
13, 145, 11, 154, 1, 150, 13, 146, 10, 156, 1, 151, 13, 148, 10, 157, 1, 152, 12, 149, 10, 159, 1,
153, 12, 150, 10, 160, 1, 155, 12, 152, 10, 162, 1, 156, 12, 153, 9, 163, 1, 157, 12, 154, 9, 165,
1, 158, 12, 156, 9, 166, 1, 160, 12, 157, 9, 168, 1, 161, 11, 158, 9, 169, 1, 162, 11, 160, 8, 171,
1, 164, 11, 161, 8, 172, 1, 163, 13, 161, 10, 172, 2, 163, 14, 161, 11, 172, 4, 163, 15, 161, 12,
172, 6, 163, 17, 161, 13, 172, 7, 163, 18, 160, 15, 172, 8, 163, 19, 160, 16, 172, 10, 162, 21,
160, 17, 172, 12, 162, 22, 160, 18, 172, 13, 162, 23, 160, 20, 172, 14, 162, 24, 160, 21, 172, 16,
162, 26, 160, 22, 172, 18, 161, 27, 160, 23, 172, 19, 161, 28, 160, 25, 172, 20, 161, 29, 159, 26,
172, 22, 161, 31, 159, 27, 172, 24, 161, 32, 159, 28, 172, 25, 161, 33, 159, 29, 172, 26, 160, 34,
159, 31, 172, 28, 160, 35, 159, 32, 172, 30, 160, 37, 159, 33, 172, 31, 160, 38, 159, 34, 172, 32,
160, 39, 159, 35, 172, 34, 160, 40, 159, 36, 172, 36, 160, 41, 159, 37, 172, 37, 159, 42, 159, 38,
172, 38, 159, 44, 159, 40, 172, 40, 159, 45, 159, 41, 172, 42, 159, 46, 159, 42, 172, 43, 159, 47,
159, 43, 172, 44, 159, 48, 159, 44, 172, 46};

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

void mui_InitInternalState(mui_state_ptr instance)
{
    instance->__internal.elapsed_time_ms = 0;
}

void mui_Update(mui_state_ptr instance, uint32_t delta_ms)
{
    instance->__internal.elapsed_time_ms += delta_ms;
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
    mui_FixedPointToStr(buffer, instance->vehicle_speed, 0);
    mgl_DrawTextCenter(gl, buffer, FNT_SPEEDO, 93, 16, 70);
}
