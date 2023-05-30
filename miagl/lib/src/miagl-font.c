#include <miagl-font.h>

#include <miagl-bitmap.h>

static uint32_t mgl_BinSearchGlyphIndex(uint8_t glyph, const mgl_glyph_t* font)
{
    uint32_t start = 1;
    uint32_t end = (uint32_t)font->glyph + 1;

    while (start < end) {
        uint32_t center = (start + end) / 2;
        uint8_t current_glyph = font[center].glyph;

        if (current_glyph == glyph) return center;
        else if (current_glyph > glyph) end = center; // left half
        else start = center; // right half
    }

    if (font[start].glyph == glyph) {
        return start;
    }

    return 0;
}

int16_t mgl_CalcTextWidth(const char* text, const mgl_glyph_t* font)
{
    int16_t total_width = 0;

    while (*text) {
        uint32_t index = mgl_BinSearchGlyphIndex(*text, font);

        if (index) {
            total_width += font[index].left_spacing;
            total_width += mgl_GetBitmapWidth(font[index].bitmap);
            total_width += font[index].right_spacing;
        }

        ++text;
    }

    return total_width;
}

void mgl_DrawText(miagl_ptr instance, const char* text,
                  const mgl_glyph_t* font, uint16_t x, uint16_t y)
{
    y += font[0].baseline; // calculate baseline y-coord
    while (*text) {
        uint32_t index = mgl_BinSearchGlyphIndex(*text, font);

        if (index) {
            register const uint32_t* current_bitmap = font[index].bitmap;

            x += font[index].left_spacing;
            mgl_DrawTintedBitmap(instance, x, y - font[index].baseline, current_bitmap);
            x += mgl_GetBitmapWidth(current_bitmap) + font[index].right_spacing;
        }

        ++text;
    }
}

void mgl_DrawTextCenter(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                        uint16_t x, uint16_t y, int16_t width)
{
    int16_t real_width = mgl_CalcTextWidth(text, font);
    mgl_DrawText(instance, text, font, x + (width - real_width) / 2, y);
}

void mgl_DrawTextRight(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                       uint16_t x, uint16_t y, int16_t width)
{
    int16_t real_width = mgl_CalcTextWidth(text, font);
    mgl_DrawText(instance, text, font, x + (width - real_width), y);
}

void mgl_DrawXorText(miagl_ptr instance, const char* text,
                     const mgl_glyph_t* font, uint16_t x, uint16_t y)
{
    y += font[0].baseline; // calculate baseline y-coord
    while (*text) {
        uint32_t index = mgl_BinSearchGlyphIndex(*text, font);

        if (index) {
            register const uint32_t* current_bitmap = font[index].bitmap;

            x += font[index].left_spacing;
            mgl_DrawXorBitmap(instance, x, y - font[index].baseline, current_bitmap);
            x += mgl_GetBitmapWidth(current_bitmap) + font[index].right_spacing;
        }

        ++text;
    }
}

void mgl_DrawXorTextCenter(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                           uint16_t x, uint16_t y, int16_t width)
{
    int16_t real_width = mgl_CalcTextWidth(text, font);
    mgl_DrawXorText(instance, text, font, x + (width - real_width) / 2, y);
}

void mgl_DrawXorTextRight(miagl_ptr instance, const char* text, const mgl_glyph_t* font, 
                          uint16_t x, uint16_t y, int16_t width)
{
    int16_t real_width = mgl_CalcTextWidth(text, font);
    mgl_DrawXorText(instance, text, font, x + (width - real_width), y);
}
