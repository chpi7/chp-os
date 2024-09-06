#include <stdio.h>

#define ASSERT(condition)                          \
    do                                             \
    {                                              \
        if (!(condition))                          \
        {                                          \
            printf("Assertion failed" #condition); \
        }                                          \
    } while (0)

#define ASSERT_MSG(condition, msg, ...)                                         \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            printf("Assertion failed: " #condition ", " msg "\n", __VA_ARGS__); \
        }                                                                       \
    } while (0)
