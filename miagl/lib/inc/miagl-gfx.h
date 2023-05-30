#ifndef _MIAGL_GFX_H_
#define _MIAGL_GFX_H_

#include "miagl-buffer.h"

void mgl_DrawHLine(miagl_ptr instance, uint16_t x1, uint16_t x2, uint16_t y);
void mgl_DrawVLine(miagl_ptr instance, uint16_t x, uint16_t y1, uint16_t y2);
void mgl_StrokeRect(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void mgl_FillRect(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void mgl_DrawLine(miagl_ptr instance, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

#endif
