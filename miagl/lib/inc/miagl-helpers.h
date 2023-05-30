#ifndef _MIAGL_HELPERS_H_
#define _MIAGL_HELPERS_H_

#include <endian.h>
#include <stdint.h>

#if __BYTE_ORDER == __BIG_ENDIAN

inline uint32_t fix_endianess(uint32_t in) { return in; }

#elif defined(__x86_64__)

inline uint32_t fix_endianess(uint32_t in)
{
    __asm__(
        "bswap %[word]"
        : [word] "+r" (in)
    );
    return in;
}

#elif defined(__arm__)

inline uint32_t fix_endianess(uint32_t in)
{
    __asm__(
        "rev %w[word], %w[word]"
        : [word] "+r" (in)
    );
    return in;
}

#endif

#endif
