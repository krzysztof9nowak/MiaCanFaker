#include <miagl-bitmap.h>
#include <miagl-helpers.h>

#include <miagl.h>

#include <stdbool.h>


static inline ALWAYS_INLINE void mgl_DrawBitmapImpl(miagl_ptr instance, uint16_t x, uint16_t y, 
                                                    const uint32_t* bitmap, const uint32_t tint, 
                                                    const bool xor)
{
    uint16_t width = (*bitmap) >> 16;
    uint16_t height = (*bitmap) & 0xFFFF;
    ++bitmap;

    uint16_t real_x2 = ((x + width) > instance->display_x) ? (instance->display_x) : x + width;
    uint16_t bmp_stride = width / 8 + ((width % 8) ? 1 : 0);

    while (height-- > 0) {
        if (y >= instance->display_y) { 
            bitmap += bmp_stride;
            y++; 
            continue; 
        }

        uint32_t firstCell = y * instance->stride32 + x / 8;
        uint32_t lastCell = y * instance->stride32 + (real_x2 - 1) / 8;
        const uint32_t* row_bitmap = bitmap;
        uint32_t* buffer = (uint32_t*)instance->current_buffer;

        uint16_t mod_offset = x & 7;
        uint16_t mod_shift = mod_offset* 4;
 
        if (firstCell == lastCell) {
            if (xor) {
                buffer[firstCell] = mgl_FixEndianess(mgl_FixEndianess(buffer[firstCell]) ^ (*bitmap >> mod_shift));
            } else {
                register uint32_t left_mask = MASKS_BE[mod_offset];
                register uint32_t right_mask = MASKS_BE[(mod_offset + width > 8) ? (8) : (mod_offset + width)];
                buffer[firstCell] = mgl_FixEndianess(
                    (~((~left_mask) & right_mask) & mgl_FixEndianess(buffer[firstCell]))
                    | ((*bitmap >> mod_shift) & (~left_mask) & right_mask & tint)
                );
            }
        } else {
            // Optimized drawing that aligns to words
            uint16_t row_width = width;
            uint32_t remaining = mod_offset ? ((*row_bitmap) << (32 - mod_shift)) : 0;

            if (xor) {
                buffer[firstCell] = mgl_FixEndianess(mgl_FixEndianess(buffer[firstCell]) ^ (*bitmap >> mod_shift));
            } else {
                buffer[firstCell] = mgl_FixEndianess(
                    (MASKS_BE[mod_offset] & mgl_FixEndianess(buffer[firstCell])) 
                    | ((*row_bitmap >> mod_shift) & (~MASKS_BE[mod_offset]) & tint)
                );
            }

            ++firstCell;
            row_width -= 8 - mod_offset;
            while (firstCell < lastCell) {
                if (xor) {
                    buffer[firstCell] = mgl_FixEndianess(
                        (remaining | ((*(++row_bitmap) >> mod_shift) & (~MASKS_BE[mod_offset]))) ^ mgl_FixEndianess(buffer[firstCell])
                    );
                    ++firstCell;
                } else {
                    buffer[firstCell++] = mgl_FixEndianess(
                        (remaining | ((*(++row_bitmap) >> mod_shift) & (~MASKS_BE[mod_offset]))) & tint
                    );
                }
                remaining = mod_offset ? ((*row_bitmap) << (32 - mod_shift)) : 0;
                row_width -= 8;
            }

            if (row_width > 8) row_width = 8; // if bitmap exceeds screen boundaries

            if (xor) {
                buffer[lastCell] = mgl_FixEndianess(
                    ((remaining | ((*(++row_bitmap) >> mod_shift) & (~MASKS_BE[mod_offset]))) & MASKS_BE[row_width]) 
                    ^ mgl_FixEndianess(buffer[lastCell])
                );
            }
            else {
                buffer[lastCell] = mgl_FixEndianess(
                    ((~MASKS_BE[row_width]) & mgl_FixEndianess(buffer[lastCell])) 
                    | ((remaining | ((*(++row_bitmap) >> mod_shift) & (~MASKS_BE[mod_offset]))) & MASKS_BE[row_width] & tint)
                );
            }
        }

        bitmap += bmp_stride;
        y++;
    }
}

void mgl_DrawBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap)
{
    mgl_DrawBitmapImpl(instance, x, y, bitmap, 0xFFFFFFFF, false); // hope that optimizer kicks in
}

void mgl_DrawTintedBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap)
{
    mgl_DrawBitmapImpl(instance, x, y, bitmap, mgl_ColorNibbleToWord(instance->color), false);
}

void mgl_DrawXorBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap)
{
    mgl_DrawBitmapImpl(instance, x, y, bitmap, 0xFFFFFFFF, true);
}
