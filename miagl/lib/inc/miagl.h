#ifndef _MIAGL_H_
#define _MIAGL_H_

#include "miagl-buffer.h"
#include "miagl-driver.h"

#include <stdbool.h>

#define MIAGL_COLOR_BLACK 0x0
#define MIAGL_COLOR_WHITE 0xF

/// Library functions

bool mgl_InitLibrary(miagl_ptr instance, uint16_t display_width, 
                     uint16_t display_height, miagl_driver_ptr driver_impl);

/**
 * \brief Set a color of a single pixel
 * 
 * \param x x-coordinate of the pixel, pass 0 for the leftmost pixel
 * \param y y-coordinate of the pixel, pass 0 for the topmost pixel
 * \param brightness new brightness value, in range 0-15
 */
void mgl_SetPixel(miagl_ptr instance, uint16_t x, uint16_t y, uint8_t brightness);
uint8_t mgl_GetPixel(miagl_ptr instance, uint16_t x, uint16_t y);

void mgl_SetBackgroundColor(miagl_ptr instance, uint8_t color);
uint8_t mgl_GetBackgroundColor(miagl_ptr instance);
void mgl_SetColor(miagl_ptr instance, uint8_t color);
uint8_t mgl_GetColor(miagl_ptr instance);
void mgl_FillScreen(miagl_ptr instance);
void mgl_FlushScreen(miagl_ptr instance);
bool mgl_SetBackgroundBitmap(miagl_ptr instance, const void* buffer, uint16_t size);
const void* mgl_GetBackgroundBitmap(miagl_ptr instance);

#endif
