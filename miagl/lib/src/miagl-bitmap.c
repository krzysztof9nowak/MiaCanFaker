#include <miagl-bitmap.h>
#include <miagl-helpers.h>

#include <miagl.h>

void mgl_DrawBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap)
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
            register uint32_t left_mask = MASKS_BE[mod_offset];
            register uint32_t right_mask = MASKS_BE[(mod_offset + width > 8) ? (8) : (mod_offset + width)];
            buffer[firstCell] = fix_endianess(
                (left_mask & (~right_mask) & fix_endianess(buffer[firstCell]))
                | ((*bitmap >> mod_shift) & (~left_mask) & right_mask)
            );
        } else {
            // Optimized drawing that aligns to words
            uint16_t row_width = width;
            uint32_t remaining = mod_offset ? ((*row_bitmap) << (32 - mod_shift)) : 0;

            buffer[firstCell] = fix_endianess(
                (MASKS_BE[mod_offset] & fix_endianess(buffer[firstCell])) 
                | ((*row_bitmap >> mod_shift) & (~MASKS_BE[mod_offset]))
            );

            ++firstCell;
            row_width -= 8 - mod_offset;
            while (firstCell < lastCell) {
                buffer[firstCell++] = fix_endianess(
                    remaining | ((*(++row_bitmap) >> mod_shift) & (~MASKS_BE[mod_offset]))
                );
                remaining = mod_offset ? ((*row_bitmap) << (32 - mod_shift)) : 0;
                row_width -= 8;
            }

            if (row_width > 8) row_width = 8;

            buffer[lastCell] = fix_endianess(
                ((~MASKS_BE[row_width]) & fix_endianess(buffer[lastCell])) 
                | ((remaining | ((*(++row_bitmap) >> mod_shift) & (~MASKS_BE[mod_offset]))) & MASKS_BE[row_width])
            );
        }

        bitmap += bmp_stride;
        y++;
    }
}
