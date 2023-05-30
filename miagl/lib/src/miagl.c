#include <miagl.h>

#ifdef __arm__
#include <machine/endian.h>
#else
#include <endian.h>
#endif
#include <memory.h>
#include <stdlib.h>

/* Helper table for faster horizontal line drawing */
#if __BYTE_ORDER == __LITTLE_ENDIAN
const uint32_t MASKS[] = { 0x00000000, 0x000000F0, 0x000000FF, 0x0000F0FF, 0x0000FFFF, 0x00F0FFFF, 0x00FFFFFF, 0xF0FFFFFF, 0xFFFFFFFF };
#elif __BYTE_ORDER == __BIG_ENDIAN
const uint32_t MASKS[] = { 0x00000000, 0xF0000000, 0xFF000000, 0xFFF00000, 0xFFFF0000, 0xFFFFF000, 0xFFFFFF00, 0xFFFFFFF0, 0xFFFFFFFF };
#endif
const uint32_t MASKS_BE[] = { 0x00000000, 0xF0000000, 0xFF000000, 0xFFF00000, 0xFFFF0000, 0xFFFFF000, 0xFFFFFF00, 0xFFFFFFF0, 0xFFFFFFFF };

static void mgl_SwapBuffers(miagl_ptr instance)
{
    void* temp = instance->current_buffer;
    instance->current_buffer = instance->previous_buffer;
    instance->previous_buffer = temp;
}

static void mgl_CallMemcpy(miagl_ptr instance, void* dst, const void* src, uint16_t size)
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
    if (instance->frame_bg) {
        mgl_CallMemcpy(instance, buffer, instance->frame_bg, instance->display_y * instance->stride);
    } else {
        uint8_t value = (instance->bgcolor << 4) | instance->bgcolor;
        mgl_CallMemset(instance, buffer, value, instance->display_y * instance->stride);
    }
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
    uint32_t index = 0;
    for (uint32_t y = start_y; y <= end_y; y++) {
        for (uint32_t x = start_x; x <= end_x; x += 8) {
            instance->dma_buffer[index++] = current[y * instance->stride32 + (x / 8)];
        }
    }

    if (instance->driver_data->fn_flush_part_screen) {
        instance->driver_data->fn_flush_part_screen(
            instance->dma_buffer, index * sizeof(uint32_t), start_x, end_x + 7, start_y, end_y);
    }
}

bool mgl_InitLibrary(miagl_ptr instance, uint16_t display_width, 
                     uint16_t display_height, miagl_driver_ptr driver_impl)
{
    memset(instance, 0, sizeof(miagl_t));
    uint16_t fboSize = display_width * display_height / 2; // 4bpp

    instance->display_x = display_width;
    instance->display_y = display_height;
    instance->stride = display_width / 2;
    instance->stride32 = display_width / 8;
    instance->driver_data = driver_impl;
    instance->frame_buffer1 = instance->static_buffer;
    instance->frame_buffer2 = instance->static_buffer + fboSize;
    instance->dma_buffer = (uint32_t*)(instance->static_buffer + fboSize * 2);
    instance->triangle_buffer = (uint32_t*)(instance->static_buffer + fboSize * 3);
    instance->current_buffer = instance->frame_buffer1;
    instance->previous_buffer = instance->frame_buffer2;
    instance->bgcolor = MIAGL_COLOR_BLACK;
    instance->color = MIAGL_COLOR_WHITE;
    instance->frame_bg = NULL;

    memset(instance->frame_buffer1, 0, fboSize);
    memset(instance->frame_buffer2, 0, fboSize);
    memset(instance->dma_buffer, 0, fboSize);
    memset(instance->triangle_buffer, 0, display_height * 4);

    if (driver_impl->fn_init_driver) {
        driver_impl->fn_init_driver();
    }
    if (driver_impl->fn_flush_full_screen) {
        driver_impl->fn_flush_full_screen(instance->dma_buffer, fboSize);
    }

    return instance->frame_buffer1 && instance->frame_buffer2;
}

void mgl_SetPixel(miagl_ptr instance, uint16_t x, uint16_t y, uint8_t brightness)
{
    if (x >= instance->display_x || y >= instance->display_y) return;
    brightness &= LOMSK;

    if (x % 2 == 1) {
        instance->current_buffer[PIX(instance, x, y)] &= HIMSK;
        instance->current_buffer[PIX(instance, x, y)] |= brightness;
    } else {
        instance->current_buffer[PIX(instance, x, y)] &= LOMSK;
        instance->current_buffer[PIX(instance, x, y)] |= (brightness << 4);
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
    mgl_CallMemset(instance, instance->current_buffer, value, instance->display_y * instance->stride);
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

bool mgl_SetBackgroundBitmap(miagl_ptr instance, const void* bitmap, uint16_t size) 
{
    if (instance->stride * instance->display_y > size) {
        return false;
    }

    instance->frame_bg = bitmap;
    return true;
}

const void* mgl_GetBackgroundBitmap(miagl_ptr instance)
{
    return instance->frame_bg;
}
