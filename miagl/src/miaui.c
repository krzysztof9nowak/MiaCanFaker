#include <miaui.h>

#include <miagl-bitmap.h>
#include <miagl-gfx.h>
#include <miaui-gauge.h>
#include <miaui-resources.h>

#include <stdio.h>


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
    .x = 30,
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
    int cell_voltage = instance->capacitor_voltage * 10 / instance->cell_count;
    sprintf(buffer, "%d.%02d", cell_voltage / 100, cell_voltage % 100);
    mgl_SetColor(gl, 11);
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 1, 1, 19);

    // Draw gauge shadows
    mgl_SetColor(gl, 8);
    mgl_DrawVLine(gl, 27, 10, 60);
    mgl_DrawHLine(gl, 23, 27, 61);
    mgl_DrawVLine(gl, 34, 10, 60);
    mgl_DrawHLine(gl, 30, 34, 61);

    // Draw volts scale
    uint16_t original_volts_coord = mui_CalculateGaugeCoord(VOLTS_GAUGE.y, VOLTS_GAUGE.height, true, 
                                                   VOLTS_GAUGE.min, VOLTS_GAUGE.max, instance->capacitor_voltage);
    uint16_t volts_coord = original_volts_coord;
    if (volts_coord < 14) volts_coord = 14;
    if (volts_coord > 57) volts_coord = 57;
    mgl_DrawBitmap(gl, 0, volts_coord - 4, IMG_SCALE_VOLTS);
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_DrawHLine(gl, 18, 23, original_volts_coord);
    sprintf(buffer, "%d.%01d", instance->capacitor_voltage / 10, instance->capacitor_voltage % 10);
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 1, volts_coord - 3, 19);

    // Draw amps scale
    uint16_t original_amps_coord = mui_CalculateGaugeCoord(AMPS_GAUGE.y, AMPS_GAUGE.height, true, 
                                                   AMPS_GAUGE.min, AMPS_GAUGE.max, instance->motor_current);
    uint16_t amps_coord = original_amps_coord;
    if (amps_coord > 57) amps_coord = 57;
    mgl_DrawBitmap(gl, 36, amps_coord - 4, IMG_SCALE_VOLTS);
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_DrawHLine(gl, 32, 36, original_amps_coord);
    sprintf(buffer, "%d", instance->motor_current);
    mgl_DrawXorTextCenter(gl, buffer, FNT_DIGITS_4X7, 37, amps_coord - 3, 20);

    // Draw electricity gauges
    mui_DrawVGauge(gl, &VOLTS_GAUGE, instance->capacitor_voltage);
    mui_DrawVGauge(gl, &AMPS_GAUGE, instance->motor_current);

}
