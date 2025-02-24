#ifndef _MIAGL_FONT_H_
#define _MIAGL_FONT_H_

#include "miagl-buffer.h"

typedef struct mgl_glyph {
    const uint32_t *bitmap;
    uint16_t left_spacing;
    uint16_t right_spacing;
    uint16_t baseline;
    uint8_t glyph;
} mgl_glyph_t;

/*
 * Font definition:
 *
 * The first glyph in the font is the control glyph - baseline field contains
 * height of the font, and glyph field contains glyph count
 * 
 * Further glyphs have to be ordered because they are traversed with binsearch
 * algorithm
 */

void mgl_SetExtraSpacing(miagl_ptr instance, int16_t spacing);
uint16_t mgl_GetExtraSpacing(miagl_ptr instance);
void mgl_ClearExtraSpacing(miagl_ptr instance);

int16_t mgl_CalcTextWidth(miagl_ptr instance, const char* text, const mgl_glyph_t* font);

void mgl_DrawText(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                  uint16_t x, uint16_t y);
void mgl_DrawTextCenter(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                        uint16_t x, uint16_t y, int16_t width);
void mgl_DrawTextRight(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                       uint16_t x, uint16_t y, int16_t width);
void mgl_DrawXorText(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                     uint16_t x, uint16_t y);
void mgl_DrawXorTextCenter(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                           uint16_t x, uint16_t y, int16_t width);
void mgl_DrawXorTextRight(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                          uint16_t x, uint16_t y, int16_t width);

#endif
