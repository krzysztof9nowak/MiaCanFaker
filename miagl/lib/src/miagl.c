#include <miagl.h>

#include <memory.h>
#include <stdlib.h>

#define PIX(ins, x, y) ((y) * (ins)->strobe + (x >> 1))
#define HIMSK 0xF0
#define LOMSK 0x0F


static void mgl_SwapBuffers(miagl_ptr instance)
{
    void* temp = instance->current_buffer;
    instance->current_buffer = instance->previous_buffer;
    instance->previous_buffer = temp;
}

static void mgl_CallMemcpy(miagl_ptr instance, void* dst, void* src, uint16_t size)
{
    if (instance->driver_data->fn_dma_memcpy) {
        instance->driver_data->fn_dma_memcpy(dst, src, size);
        while (instance->driver_data->fn_dma_is_running());
    } else {
        memcpy(dst, src, size);
    }
}

static void mgl_CallMemset(miagl_ptr instance, void* block, uint8_t value, uint16_t size)
{
    if (instance->driver_data->fn_dma_memset) {
        instance->driver_data->fn_dma_memset(block, value, size);
        while (instance->driver_data->fn_dma_is_running());
    } else {
        memset(block, value, size);
    }
}

static void mgl_ClearBuffer(miagl_ptr instance, void* buffer)
{
    uint8_t value = (instance->bgcolor << 4) | instance->bgcolor;
    mgl_CallMemset(instance, buffer, value, instance->display_y * instance->strobe);
}

static void mgl_SendBuffer(miagl_ptr instance, uint16_t start_x, uint16_t end_x,
                           uint16_t start_y, uint16_t end_y)
{
    if (start_x > end_x || start_y > end_y) return;

    // Copy to the DMA output buffer
    if (instance->driver_data->fn_is_buffer_ready) {
        while (!instance->driver_data->fn_is_buffer_ready());
    }

    uint32_t* current = (uint32_t*)instance->current_buffer;
    uint32_t strobe = instance->strobe / 8;
    uint32_t index = 0;
    for (uint32_t y = start_y; y <= end_y; y++) {
        for (uint32_t x = start_x; x <= end_x; x += 8) {
            instance->dma_buffer[index++] = current[y * strobe + (x / 8)];
        }
    }

    if (instance->driver_data->fn_flush_part_screen) {
        instance->driver_data->fn_flush_part_screen(
            instance->dma_buffer, index * sizeof(uint32_t), start_x, end_x + 3, start_y, end_y);
    }
}

bool mgl_InitLibrary(miagl_ptr instance, uint16_t display_width, 
                     uint16_t display_height, miagl_driver_ptr driver_impl)
{
    memset(instance, 0, sizeof(miagl_t));
    uint16_t fboSize = display_width * display_height / 2; // 4bpp

    instance->display_x = display_width;
    instance->display_y = display_height;
    instance->strobe = display_width / 2;
    instance->driver_data = driver_impl;
    instance->frame_buffer1 = malloc(fboSize);
    instance->frame_buffer2 = malloc(fboSize);
    instance->dma_buffer = malloc(fboSize);
    instance->current_buffer = instance->frame_buffer1;
    instance->previous_buffer = instance->frame_buffer2;
    instance->bgcolor = MIAGL_COLOR_BLACK;
    instance->color = MIAGL_COLOR_WHITE;

    memset(instance->frame_buffer1, 0, fboSize);
    memset(instance->frame_buffer2, 0, fboSize);
    memset(instance->dma_buffer, 0, fboSize);

    if (driver_impl->fn_init_driver) {
        driver_impl->fn_init_driver();
    }
    if (driver_impl->fn_flush_full_screen) {
        driver_impl->fn_flush_full_screen(instance->dma_buffer, fboSize);
    }

    return instance->frame_buffer1 && instance->frame_buffer2;
}

void mgl_FreeLibrary(miagl_ptr instance)
{
    if (instance->frame_buffer1) free(instance->frame_buffer1);
    if (instance->frame_buffer2) free(instance->frame_buffer2);
    if (instance->dma_buffer) free(instance->dma_buffer);

    memset(instance, 0, sizeof(miagl_t));
}

void mgl_SetPixel(miagl_ptr instance, uint16_t x, uint16_t y, uint8_t brightness)
{
    if (x >= instance->display_x || y >= instance->display_y) return;
    brightness &= LOMSK;

    if (x % 2 == 1) {
        instance->current_buffer[PIX(instance, x, y)] &= LOMSK;
        instance->current_buffer[PIX(instance, x, y)] |= (brightness << 4);
    } else {
        instance->current_buffer[PIX(instance, x, y)] &= HIMSK;
        instance->current_buffer[PIX(instance, x, y)] |= brightness;
    }
}

uint8_t mgl_GetPixel(miagl_ptr instance, uint16_t x, uint16_t y)
{
    if (x >= instance->display_x || y >= instance->display_y) return 0;
    if (x % 2 == 1) {
        return (instance->current_buffer[PIX(instance, x, y)] & HIMSK) >> 4;
    } else {
        return (instance->current_buffer[PIX(instance, x, y)] & LOMSK);
    }
}

void mgl_SetBackgroundColor(miagl_ptr instance, uint8_t color)
{
    instance->bgcolor = color & LOMSK;
}

uint8_t mgl_GetBackgroundColor(miagl_ptr instance)
{
    return instance->bgcolor;
}

void mgl_SetColor(miagl_ptr instance, uint8_t color)
{
    instance->color = color & LOMSK;
}

uint8_t mgl_GetColor(miagl_ptr instance)
{
    return instance->color;
}

void mgl_FillScreen(miagl_ptr instance)
{
    uint8_t value = (instance->color << 4) | instance->color;
    mgl_CallMemset(instance, instance->current_buffer, value, instance->display_y * instance->strobe);
}

void mgl_FlushScreen(miagl_ptr instance)
{
    uint16_t start_x = instance->display_x;
    uint16_t start_y = instance->display_y;
    uint16_t end_x = 0;
    uint16_t end_y = 0;

    uint32_t* current = (uint32_t*)instance->current_buffer;
    uint32_t* previous = (uint32_t*)instance->previous_buffer;
    
    // Find a bounding rectangle of changed area
    // to shorten up transfers to peripherals
    for (uint32_t y = 0, index = 0; y < instance->display_y; y++) {
        for (uint32_t x = 0; x < instance->display_x; x += 8, index++) {
            if (current[index] != previous[index]) {
                if (x < start_x) start_x = x;
                if (x > end_x) end_x = x;
                if (y < start_y) start_y = y;
                if (y > end_y) end_y = y;
            }
        }
    }

    mgl_SendBuffer(instance, start_x, end_x, start_y, end_y);
    mgl_SwapBuffers(instance);
    mgl_ClearBuffer(instance, instance->current_buffer);
}
