#include <miagl-gfx.h>

#include <miagl.h>

#include <stdbool.h>
#include <stdlib.h>

#define ABS(x) ((x < 0) ? (-(x)) : (x))

/* Gamma correction table for antialiased lines */
static const uint8_t GAMMA_LUT[] = {0, 18, 25, 31, 35, 39, 43, 46, 49, 
    52, 55, 57, 59, 62, 64, 66, 68, 70, 72, 74, 76, 78, 79, 81, 83, 84, 
    86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 
    108, 109, 110, 112, 113, 114, 115, 116, 117, 118, 120, 121, 122, 123, 
    124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 
    138, 139, 140, 141, 141, 142, 143, 144, 145, 146, 147, 148, 149, 149, 
    150, 151, 152, 153, 154, 154, 155, 156, 157, 158, 159, 159, 160, 161, 
    162, 163, 163, 164, 165, 166, 166, 167, 168, 169, 169, 170, 171, 172, 
    172, 173, 174, 175, 175, 176, 177, 177, 178, 179, 180, 180, 181, 182, 
    182, 183, 184, 184, 185, 186, 186, 187, 188, 188, 189, 190, 190, 191, 
    192, 192, 193, 194, 194, 195, 196, 196, 197, 197, 198, 199, 199, 200, 
    201, 201, 202, 202, 203, 204, 204, 205, 205, 206, 207, 207, 208, 208, 
    209, 210, 210, 211, 211, 212, 213, 213, 214, 214, 215, 215, 216, 217, 
    217, 218, 218, 219, 219, 220, 221, 221, 222, 222, 223, 223, 224, 224, 
    225, 226, 226, 227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 
    232, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238, 239, 239, 
    240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246, 246, 
    247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253, 253, 
    254, 254, 255, 255
};

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

static inline void mgl_BlendPixel(miagl_ptr instance, uint16_t x, uint16_t y, uint8_t brightness, uint8_t alpha)
{
    if (x >= instance->display_x || y >= instance->display_y) return;

    if (x % 2 == 1) {
        uint8_t current = instance->current_buffer[PIX(instance, x, y)] & LOMSK;
        uint16_t color = (current * (~alpha & 0xFF)) + (alpha * brightness);
        color /= 255;

        instance->current_buffer[PIX(instance, x, y)] &= HIMSK;
        instance->current_buffer[PIX(instance, x, y)] |= (uint8_t)color;
    } else {
        uint8_t current = instance->current_buffer[PIX(instance, x, y)] & HIMSK;
        uint16_t color = (current >> 4) * (~alpha & 0xFF) + (alpha * brightness);
        color /= 255;

        instance->current_buffer[PIX(instance, x, y)] &= LOMSK;
        instance->current_buffer[PIX(instance, x, y)] |= (uint8_t)(color << 4);
    }
}

static inline void mgl_UpdateTriangleBuffer(miagl_ptr instance, uint16_t xmin, uint16_t xmax, uint16_t y)
{
    if (y < instance->display_y) {
        uint32_t value = instance->triangle_buffer[y];
        uint16_t min_value = value & 0xFFFF;
        uint16_t max_value = value >> 16;
        if (xmin < min_value) min_value = xmin;
        if (xmax > max_value) max_value = xmax;

        instance->triangle_buffer[y] = (max_value << 16) | (min_value);
    }
}

static void mgl_DrawLineImpl(miagl_ptr instance, uint16_t x1, uint16_t y1, 
                             uint16_t x2, uint16_t y2, bool fill_triangle_buffer)
{
    // Bresenham implementation
    // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

    if (y1 == y2) {
        mgl_DrawHLine(instance, x1, x2, y1);
        return;
    } 
    if (x1 == x2) {
        mgl_DrawVLine(instance, x1, y1, y2);

        if (fill_triangle_buffer) {
            if (y1 > y2) {
                uint16_t temp = y2;
                y2 = y1;
                y1 = temp;
            }

            while (y1 <= y2) {
                mgl_UpdateTriangleBuffer(instance, x1, x1, y1++);
            }
        }

        return;
    }

    uint8_t brightness = instance->color & LOMSK;

    int16_t dx = ABS(x2 - x1);
    int16_t sx = x1 < x2 ? 1 : -1;
    int16_t dy = -ABS(y2 - y1);
    int16_t sy = y1 < y2 ? 1 : -1;
    int16_t error = dx + dy;

    while (true) {
        mgl_SetPixel(instance, x1, y1, brightness);

        if (fill_triangle_buffer) mgl_UpdateTriangleBuffer(instance, x1, x1, y1);

        if (x1 == x2 && y1 == y2) return;

        int16_t e2 = 2 * error;
        if (e2 >= dy) {
            if (x1 == x2) return;
            error += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            if (y1 == y2) return;
            error += dx;
            y1 += sy;
        }
    }
}

static void mgl_DrawLineAAImpl(miagl_ptr instance, uint16_t x1, uint16_t y1, 
                               uint16_t x2, uint16_t y2, bool fill_triangle_buffer)
{
    // Xiaolin Wu's algorithm implementation
    // https://dl.acm.org/doi/pdf/10.1145/127719.122734

    if (y1 == y2) {
        mgl_DrawHLine(instance, x1, x2, y1);
        return;
    } 
    if (x1 == x2) {
        mgl_DrawVLine(instance, x1, y1, y2);

        if (fill_triangle_buffer) {
            if (y1 > y2) {
                uint16_t temp = y2;
                y2 = y1;
                y1 = temp;
            }

            while (y1 <= y2) {
                mgl_UpdateTriangleBuffer(instance, x1, x1, y1++);
            }
        }
        return;
    }

    uint8_t brightness = instance->color & LOMSK;

    uint32_t OVERFLOW = 1 << 16;
    uint32_t error = 0;
    int16_t dx = ABS(x2 - x1);
    int16_t sx = x1 < x2 ? 1 : -1;
    int16_t dy = ABS(y2 - y1);
    int16_t sy = y1 < y2 ? 1 : -1;
    bool steep = dy > dx;
    uint32_t d;

    if (steep) {
        d = ((OVERFLOW * dx + dy / 2) / dy);
    } else {
        d = ((OVERFLOW * dy + dx / 2) / dx);
    }

    uint16_t *stop1, *stop2;
    if (steep) {
        stop1 = (y1 < y2) ? &y1 : &y2;
        stop2 = (y1 < y2) ? &y2 : &y1;
    } else {
        stop1 = (x1 < x2) ? &x1 : &x2;
        stop2 = (x1 < x2) ? &x2 : &x1;
    }

    mgl_SetPixel(instance, x1, y1, brightness);
    mgl_SetPixel(instance, x2, y2, brightness);

    if (fill_triangle_buffer) {
        mgl_UpdateTriangleBuffer(instance, x1, x1, y1);
        mgl_UpdateTriangleBuffer(instance, x2, x2, y2);
    }

    while (true) {
        if (steep) {
            y1 += sy;
            y2 -= sy;
        } else {
            x1 += sx;
            x2 -= sx;
        }

        if (*stop1 > *stop2) {
            if (fill_triangle_buffer && !steep && (y2 - y1 == 2 || y1 - y2 == 2)) {
                // Triangle drawing edge-case
                mgl_UpdateTriangleBuffer(instance, x1, x1, (y1 + y2) / 2);
            }
            return;
        }

        error += d;
        if (error >= OVERFLOW) {
            error &= 0xFFFF;

            if (steep) {
                x1 += sx;
                x2 -= sx;
            } else {
                y1 += sy;
                y2 -= sy;
                
                if (fill_triangle_buffer) {
                    mgl_UpdateTriangleBuffer(instance, x1, x1, y1);
                    mgl_UpdateTriangleBuffer(instance, x2, x2, y2);
                }
            }
        }

        uint8_t this_brightness = error >> 8;
        uint8_t rev_brightness = ~this_brightness;
        mgl_BlendPixel(instance, x1, y1, brightness, GAMMA_LUT[rev_brightness]);
        mgl_BlendPixel(instance, x2, y2, brightness, GAMMA_LUT[rev_brightness]);
        
        if (steep) {
            mgl_BlendPixel(instance, x1 + sx, y1, brightness, GAMMA_LUT[this_brightness]);
            mgl_BlendPixel(instance, x2 - sx, y2, brightness, GAMMA_LUT[this_brightness]);
        } else {
            mgl_BlendPixel(instance, x1, y1 + sy, brightness, GAMMA_LUT[this_brightness]);
            mgl_BlendPixel(instance, x2, y2 - sy, brightness, GAMMA_LUT[this_brightness]);
        }

        if (fill_triangle_buffer) {
            if (steep) {
                mgl_UpdateTriangleBuffer(instance, (sx < 0) ? x1 : (x1 + 1), (sx < 0) ? (x1 - 1) : x1, y1);
                mgl_UpdateTriangleBuffer(instance, (sx > 0) ? x2 : (x2 + 1), (sx > 0) ? (x2 - 1) : x2, y2);
            }
        }
    }
}

void mgl_DrawHLine(miagl_ptr instance, uint16_t x1, uint16_t x2, uint16_t y)
{
    if (x1 > x2) {
        uint16_t temp = x2;
        x2 = x1;
        x1 = temp;
    }

    if (x2 >= instance->display_x) x2 = instance->display_x - 1;
    if (x1 >= instance->display_x || y >= instance->display_y) return;

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
    mgl_DrawLineImpl(instance, x1, y1, x2, y2, false);
}

void mgl_DrawLineAA(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    mgl_DrawLineAAImpl(instance, x1, y1, x2, y2, false);
}

void mgl_FillTriangle(miagl_ptr instance, uint16_t x1, uint16_t y1, 
                      uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
    uint16_t y_min = (y1 < y2) ? y1 : y2;
    if (y3 < y_min) y_min = y3;
    uint16_t y_max = (y1 > y2) ? y1 : y2;
    if (y3 > y_max) y_max = y3;

    if (y_min >= instance->display_y) return;
    if (y_max >= instance->display_y) y_max = instance->display_y;

    for (uint16_t y = y_min; y <= y_max; y++) {
        instance->triangle_buffer[y] = 0x0000FFFF;
    }

    mgl_DrawLineImpl(instance, x1, y1, x2, y2, true);
    mgl_DrawLineImpl(instance, x1, y1, x3, y3, true);
    mgl_DrawLineImpl(instance, x2, y2, x3, y3, true);

    for (uint16_t y = y_min; y <= y_max; y++) {
        uint32_t value = instance->triangle_buffer[y];
        uint16_t min_value = value & 0xFFFF;
        uint16_t max_value = value >> 16;

        mgl_DrawHLine(instance, min_value, max_value, y);
    }
}

void mgl_FillTriangleAA(miagl_ptr instance, uint16_t x1, uint16_t y1, 
                      uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
    uint16_t y_min = (y1 < y2) ? y1 : y2;
    if (y3 < y_min) y_min = y3;
    uint16_t y_max = (y1 > y2) ? y1 : y2;
    if (y3 > y_max) y_max = y3;

    if (y_min >= instance->display_y) return;
    if (y_max >= instance->display_y) y_max = instance->display_y;

    for (uint16_t y = y_min; y <= y_max; y++) {
        instance->triangle_buffer[y] = 0x0000FFFF;
    }

    mgl_DrawLineAAImpl(instance, x1, y1, x2, y2, true);
    mgl_DrawLineAAImpl(instance, x1, y1, x3, y3, true);
    mgl_DrawLineAAImpl(instance, x2, y2, x3, y3, true);

    for (uint16_t y = y_min; y <= y_max; y++) {
        uint32_t value = instance->triangle_buffer[y];
        uint16_t min_value = value & 0xFFFF;
        uint16_t max_value = value >> 16;

        mgl_DrawHLine(instance, min_value, max_value, y);
    }
}
