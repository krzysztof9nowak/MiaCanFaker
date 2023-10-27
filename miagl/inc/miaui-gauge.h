#ifndef _MIAUI_GAUGE_H_
#define _MIAUI_GAUGE_H_

#include <miagl.h>

#include <stdbool.h>
#include <stdint.h>

typedef struct mui_gauge_def {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t center_pt;
    int32_t min;
    int32_t max;
    bool reverse;
    uint8_t colors[];
} mui_gauge_def_t, *mui_gauge_def_ptr;


inline uint16_t mui_CalculateGaugeCoord(uint16_t pos, uint16_t dim, bool reverse,
                                        int32_t min, int32_t max, int32_t val)
{
    if (val < min) return reverse ? (pos + dim - 1) : (pos);
    if (val > max) return reverse ? (pos) : (pos + dim - 1);

    if (reverse) {
        return pos + (uint16_t)((max - val) * (int32_t)(dim - 1) / (max - min));
    }
    return pos + (uint16_t)((val - min) * (int32_t)(dim - 1) / (max - min));
}

void mui_DrawVGauge(miagl_ptr gl, const mui_gauge_def_t *def, int32_t value);
void mui_DrawHGauge(miagl_ptr gl, const mui_gauge_def_t *def, int32_t value);

#endif
