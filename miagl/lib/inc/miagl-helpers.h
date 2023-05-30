#ifndef _MIAGL_HELPERS_H_
#define _MIAGL_HELPERS_H_

#ifdef __arm__
#include <machine/endian.h>
#else
#include <endian.h>
#endif
#include <stdint.h>

#if defined(__clang__) || defined(__gcc__)

#define ALWAYS_INLINE __attribute__((always_inline))

#else

#define ALWAYS_INLINE

#endif

#if __BYTE_ORDER == __BIG_ENDIAN

inline uint32_t mgl_FixEndianess(uint32_t in) { return in; }

#elif defined(__x86_64__)

inline uint32_t mgl_FixEndianess(uint32_t in)
{
    __asm__(
        "bswap %[word]"
        : [word] "+r" (in)
    );
    return in;
}

#elif defined(__arm__)

inline uint32_t mgl_FixEndianess(uint32_t in)
{
    __asm__(
        "rev %w[word], %w[word]"
        : [word] "+r" (in)
    );
    return in;
}

#endif

inline uint32_t mgl_ColorNibbleToWord(uint8_t color)
{
    uint32_t colorWord = color & LOMSK;

    return (
        colorWord << 0 |
        colorWord << 4 |
        colorWord << 8 | 
        colorWord << 12 | 
        colorWord << 16 | 
        colorWord << 20 |
        colorWord << 24 |
        colorWord << 28
    );
}

#endif
