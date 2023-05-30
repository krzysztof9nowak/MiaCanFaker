#ifndef _MIAGL_H_
#define _MIAGL_H_

#include "miagl-driver.h"

#define MIAGL_COLOR_BLACK 0x0
#define MIAGL_COLOR_WHITE 0xF

typedef struct miagl {
    uint16_t display_x;
    uint16_t display_y;
    uint32_t strobe;
    uint8_t* frame_buffer1;
    uint8_t* frame_buffer2;
    uint32_t* dma_buffer;
    uint8_t* current_buffer;
    uint8_t* previous_buffer;
    uint8_t bgcolor;
    uint8_t color;
    miagl_driver_ptr driver_data;
} miagl_t, *miagl_ptr;


/// Library functions

bool mgl_InitLibrary(miagl_ptr instance, uint16_t display_width, 
                     uint16_t display_height, miagl_driver_ptr driver_impl);
void mgl_FreeLibrary(miagl_ptr instance);

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

#endif
