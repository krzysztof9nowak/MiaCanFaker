#ifndef _MIAUI_RESOURCES_H_
#define _MIAUI_RESOURCES_H_
#include <stddef.h>
#include <stdint.h>

// DO NOT CHANGE THIS FILE!
// RUN scripts/make_resources.py TO REGENERATE IT!

typedef struct mgl_glyph {
    const uint32_t *bitmap;
    uint16_t left_spacing;
    uint16_t right_spacing;
    uint16_t baseline;
    uint8_t glyph;
} mgl_glyph_t;


extern const uint32_t IMG_ABS[];
extern const uint32_t IMG_GLPATTERN[];
extern const uint32_t IMG_KOT[];
extern const uint32_t IMG_SMALL[];
extern const uint32_t IMG_TEST[];

extern const mgl_glyph_t FNT_DIGITS[];

#endif
