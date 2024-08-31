#include "string.h"

void memmove(void* dst, const void* src, size_t n)
{
    const uint8_t* from = (uint8_t*)src;
    uint8_t* to = (uint8_t*)dst;

    if (from < to && from + n >= to)
    {
        /* overlap:
         * <--- src --->
         *      <--- dst --->
         *
         *  ==> copy in reverse
         */
        for (int i = n - 1; i >= 0; --i)
        {
            to[i] = from[i];
        }
    }
    else
    {
        /* no overlap OR
         * overlap:
         *        <--- src --->
         * <--- dst --->
         *
         *  ==> copy forward
         */
        for (size_t i = 0; i < n; ++i)
        {
            to[i] = from[i];
        }
    }
}

void memcpy(void* dst, const void* src, size_t n) { memmove(dst, src, n); }

void memset(void *d, char c, size_t n)
{
    char* dst = (char*)d;
    for (size_t i = 0; i < n; ++i)
    {
        dst[i] = c;
    }
}
