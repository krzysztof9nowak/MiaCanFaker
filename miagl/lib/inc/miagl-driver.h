#ifndef _MIAGL_DRIVER_H_
#define _MIAGL_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct miagl_driver {
    
    /** \brief Inits the driver */
    void (*fn_init_driver)();

    /**
    \brief Checks if out buffer is ready to be replaced.

    If this function returns `false`, a spinlock will start. You can call
    several OS kernel functions in its implementation.
    */
    bool (*fn_is_buffer_ready)();

    /** \brief Send the whole frame to the display */
    void (*fn_flush_full_screen)(void* buffer, uint16_t size);

    /** \brief Send a rectangular fragment of the frame to the display */
    void (*fn_flush_part_screen)(void* buffer, uint16_t size, 
                                 uint8_t start_x, uint8_t end_x, uint8_t start_y,
                                 uint8_t end_y);

    /** 
    \brief Returns true if any library DMA operation is currently running. 
     
    It **must** be implemented if any other DMA function is also implemented.

    If this function returns `false`, a spinlock will start. You can call
    several OS kernel functions in its implementation.
    */
    bool (*fn_dma_is_running)();

    /** \brief Set the entire block of memory to the provided value using DMA */
    void (*fn_dma_memset)(void* block, uint8_t value, uint16_t size);

    /** \brief Copy the entire block of memory using DMA */
    void (*fn_dma_memcpy)(void* dst, const void* src, uint16_t size);

} miagl_driver_t, *miagl_driver_ptr;

#endif
