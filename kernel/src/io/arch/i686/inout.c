#include "io/inout.h"

inline __attribute__((always_inline)) void outb(uint16_t port, uint8_t val)
{
    // clang-format off
    __asm__ volatile("outb %0, %1"
                :
                : "a"(val), "Nd"(port));
    // clang-format on
}

inline __attribute__((always_inline)) uint8_t inb(uint16_t port)
{
    uint8_t result;
    // clang-format off
    __asm__ volatile("inb %1, %0" 
            : "=a"(result)
            : "Nd"(port));
    // clang-format on
    return result;
}
