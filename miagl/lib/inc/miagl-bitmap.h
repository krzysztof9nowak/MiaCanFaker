#ifndef _MIAGL_BITMAP_H_
#define _MIAGL_BITMAP_H_

#include "miagl-buffer.h"

#include <stdint.h>

void mgl_DrawBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap);
void mgl_DrawTintedBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap);
void mgl_DrawXorBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap);
void mgl_DrawXorTintedBitmap(miagl_ptr instance, uint16_t x, uint16_t y, const uint32_t* bitmap);

inline uint16_t mgl_GetBitmapWidth(const uint32_t* bitmap)
{
    return (*bitmap) >> 16;
}

inline uint16_t mgl_GetBitmapHeight(const uint32_t* bitmap)
{
    return (*bitmap) & 0xFFFF;
}
 
#endif
