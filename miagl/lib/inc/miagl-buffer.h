#ifndef _MIAGL_BUFFER_H_
#define _MIAGL_BUFFER_H_

#include "miagl-driver.h"

#include <stdint.h>

#define DEFINE_MIAGL_STRUCT(width, height) \
    typedef struct miagl { \
        uint16_t display_x; \
        uint16_t display_y; \
        uint32_t stride; \
        uint32_t stride32; \
        uint8_t* frame_buffer1; \
        uint8_t* frame_buffer2; \
        uint32_t* dma_buffer; \
        uint8_t* current_buffer; \
        uint8_t* previous_buffer; \
        uint32_t* triangle_buffer; \
        uint8_t bgcolor; \
        uint8_t color; \
        miagl_driver_ptr driver_data; \
        void* frame_bg; \
        uint8_t static_buffer[(width) * (height) / 2 * 3 + height * sizeof(uint16_t) * 2]; \
    } miagl_t

#ifdef MIAGL_IMPL
DEFINE_MIAGL_STRUCT(0, 0);
typedef miagl_t *miagl_ptr;

#define PIX(ins, x, y) ((y) * (ins)->stride + (x >> 1))
#define HIMSK 0xF0
#define LOMSK 0x0F
extern const uint32_t MASKS[9];
#else
typedef void *miagl_ptr;
#endif

#endif
