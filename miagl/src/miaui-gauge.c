#include <miaui-gauge.h>

#include <miagl-gfx.h>


void mui_DrawVGauge(miagl_ptr gl, const mui_gauge_def_t *def, int32_t value)
{   
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_StrokeRect(gl, def->x - 1, def->y - 1, def->x + def->width, def->y + def->height);
    
    uint16_t gauge_coord = mui_CalculateGaugeCoord(def->y, def->height, def->reverse, def->min, def->max, value);
    for (uint16_t x = 0; x < def->width; x++) {
        mgl_SetColor(gl, def->colors[x]);
        mgl_DrawVLine(gl, def->x + x, gauge_coord, def->center_pt);
    }

    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_DrawHLine(gl, def->x, def->x + def->width - 1, def->center_pt);
}

void mui_DrawHGauge(miagl_ptr gl, const mui_gauge_def_t *def, int32_t value)
{
    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_StrokeRect(gl, def->x - 1, def->y - 1, def->x + def->width, def->y + def->height);
    
    uint16_t gauge_coord = mui_CalculateGaugeCoord(def->x, def->width, def->reverse, def->min, def->max, value);
    for (uint16_t y = 0; y < def->height; y++) {
        mgl_SetColor(gl, def->colors[y]);
        mgl_DrawHLine(gl, gauge_coord, def->center_pt, def->y + y);
    }

    mgl_SetColor(gl, MIAGL_COLOR_WHITE);
    mgl_DrawVLine(gl, def->center_pt, def->y, def->y + def->height - 1);
}
