#include "log/log.h"

#include <stdio.h>

log_level log_current_level = LOG_LEVEL_DEBUG;

log_level log_get_current_level()
{
    return log_current_level;
}

/* This is broken because of the vararg handling probably
void log_debug(const char* module, const char* fmt, ...)
{
    if (LOG_LEVEL_DEBUG < log_current_level)
    {
        return;
    }

    printf("[%s]: ", module);

    va_list args;
    va_start(args, fmt);

    printf(fmt, args);

    va_end(args);
}
*/

