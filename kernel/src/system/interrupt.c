#include "system/interrupt.h"

#include "log/log.h"

#define SYS_LOG_MOD "sys"

void idt_init()
{
    LOG_DEBUG(SYS_LOG_MOD, "init idt: TODO!");
    /**
     * TODO: create 32 first interrupts handler so that we can handle the pre-defined interrupts.
     * Figure 21-2
     */
}
