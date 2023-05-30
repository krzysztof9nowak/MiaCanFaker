#include <miagl-bitmap.h>

#include <miagl.h>

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

void mgl_DrawBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap)
{
    uint16_t width = (*bitmap) >> 16;
    uint16_t height = (*bitmap) & 0xFFFF;
    ++bitmap;

    for (uint16_t y1 = 0; y1 < height; y1++) {
        uint32_t current_word = *bitmap;
        for (uint16_t x1 = 0; x1 < width; x1++) {
            mgl_SetPixel(instance, x1 + x, y1 + y, current_word & LOMSK);
            current_word >>= 4;

            if (x1 % 8 == 7) {
                ++bitmap;
                current_word = *bitmap;
            }
        }

        if (width % 8) {
            ++bitmap;
        }
    }
}
