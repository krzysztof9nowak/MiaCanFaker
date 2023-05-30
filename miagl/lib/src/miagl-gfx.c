#include <miagl-gfx.h>

#include <stdlib.h>

#define ABS(x) ((x < 0) ? (-(x)) : (x))


static inline void mgl_SetPixelUnchecked(miagl_ptr instance, uint16_t x, uint16_t y, uint8_t brightness)
{
    if (x % 2 == 1) {
        instance->current_buffer[PIX(instance, x, y)] &= HIMSK;
        instance->current_buffer[PIX(instance, x, y)] |= brightness;
    } else {
        instance->current_buffer[PIX(instance, x, y)] &= LOMSK;
        instance->current_buffer[PIX(instance, x, y)] |= (brightness << 4);
    }
}

static inline uint32_t mgl_ColorNibbleToWord(miagl_ptr instance)
{
    uint32_t colorWord = instance->color & LOMSK;

    return (
        colorWord << 0 |
        colorWord << 4 |
        colorWord << 8 | 
        colorWord << 12 | 
        colorWord << 16 | 
        colorWord << 20 |
        colorWord << 24 |
        colorWord << 28
    );
}

void mgl_DrawHLine(miagl_ptr instance, uint16_t x1, uint16_t x2, uint16_t y)
{
    if (x1 > x2) {
        uint16_t temp = x2;
        x2 = x1;
        x1 = temp;
    }

    if (x2 > instance->display_x) x2 = instance->display_x - 1;
    if (x1 > instance->display_x || y > instance->display_y) return;

    uint32_t firstCell = y * instance->stride32 + x1 / 8;
    uint32_t lastCell = y * instance->stride32 + x2 / 8;
    uint8_t brightness = instance->color & LOMSK;

    if (firstCell == lastCell) { 
        // This is a short line, no need to do aligned word optimization
        for (uint16_t x = x1; x <= x2; ++x) {
            mgl_SetPixelUnchecked(instance, x, y, brightness);
        }
    } else {
        uint32_t* buffer = (uint32_t*)instance->current_buffer;
        uint32_t colorData = mgl_ColorNibbleToWord(instance);
        uint8_t remainder1 = x1 & 7;
        uint8_t remainder2 = (x2 & 7) + 1;

        buffer[firstCell] = (buffer[firstCell] & (MASKS[remainder1])) | (colorData & (~MASKS[remainder1]));
        ++firstCell;
        while (firstCell < lastCell) {
            buffer[firstCell++] = colorData;
        }
        buffer[lastCell] = (colorData & MASKS[remainder2]) | (buffer[lastCell] & (~MASKS[remainder2]));
    }
}

void mgl_DrawVLine(miagl_ptr instance, uint16_t x, uint16_t y1, uint16_t y2)
{
    if (y1 > y2) {
        uint16_t temp = y2;
        y2 = y1;
        y1 = temp;
    }

    if (y2 > instance->display_y) y2 = instance->display_y - 1;
    if (y1 > instance->display_y || x > instance->display_x) return;

    uint8_t brightness = instance->color & LOMSK;
    while (y1 <= y2) {
        mgl_SetPixelUnchecked(instance, x, y1++, brightness);
    }
}

void mgl_StrokeRect(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    mgl_DrawHLine(instance, x1, x2, y1);
    mgl_DrawVLine(instance, x1, y1, y2);
    mgl_DrawVLine(instance, x2, y1, y2);
    mgl_DrawHLine(instance, x1, x2, y2);
}

void mgl_FillRect(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    while (y1 <= y2) {
        mgl_DrawHLine(instance, x1, x2, y1++);
    }
}

void mgl_DrawLine(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // Bresenham implementation
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    if (y1 == y2) {
        mgl_DrawHLine(instance, x1, x2, y1);
        return;
    } 
    if (x1 == x2) {
        mgl_DrawVLine(instance, x1, y1, y2);
        return;
    }

    uint8_t brightness = instance->color & LOMSK;

    int16_t dx = ABS(x2 - x1);
    int16_t sx = x1 < x2 ? 1 : -1;
    int16_t dy = -ABS(y2 - y1);
    int16_t sy = y1 < y2 ? 1 : -1;
    int16_t error = dx + dy;

    while (true) {
        mgl_SetPixelUnchecked(instance, x1, y1, brightness);

        if (x1 == x2 && y1 == y2) break;

        int16_t e2 = 2 * error;
        if (e2 >= dy) {
            if (x1 == x2) break;
            error += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            if (y1 == y2) break;
            error += dx;
            y1 += sy;
        }
    }
}
