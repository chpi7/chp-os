#include <stdarg.h>
#include <stdio.h>

/**
 * Higher number --> more verbose
 */
typedef enum log_level
{
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} log_level;

log_level log_get_current_level();

#define LOG_DEBUG(mod, fmt, ...)                        \
    do                                                  \
    {                                                   \
        if (log_get_current_level() >= LOG_LEVEL_DEBUG) \
        {                                               \
            printf("[" mod "]: " fmt "\n", ##__VA_ARGS__); \
        }                                               \
    } while (0)
