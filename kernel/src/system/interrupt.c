#include "system/interrupt.h"

#include <stdio.h>

#include "log/log.h"
#include "system/segment.h"
#include "common/assert.h"

#define SYS_LOG_MOD "sys"

idtr_t idtr;
idt_t idt;

extern uint32_t isr_dispatch_table[IDT_MAX_ENTRIES];
extern idt_isr_t isr_user_handlers[IDT_MAX_ENTRIES];

void isr_0_31(uint32_t vector)
{
    printf("received interrupt %u\n", vector);
}

#define IDT_DESC_PRESENT (1 << 15)
#define IDT_DESC_TYPE_INT (0x3 << 9)
#define IDT_DESC_GATE_SIZE_32 (1 << 11)

void idt_init_interrupt_gate(idt_descriptor_t *desc, uint32_t offset, uint16_t seg, uint16_t dpl)
{
    desc->flags = ((dpl & 0x3) << 13) | IDT_DESC_PRESENT | IDT_DESC_TYPE_INT | IDT_DESC_GATE_SIZE_32;
    desc->cs = seg;
    desc->offset_0_15 = offset & 0xFFFF;
    desc->offset_16_31 = (uint16_t)((offset >> 16) & 0xFFFF);
}

extern void idtr_reload();

void idt_flush()
{
    idtr.limit = (sizeof(idt_descriptor_t) * IDT_MAX_ENTRIES) - 1;
    idtr.base = (uint32_t)&idt.entries[0];

    idtr_reload();

    LOG_DEBUG(SYS_LOG_MOD, "Flushed IDT");
}

void idt_init()
{
    /* Setup system handlers. */
    for (uint32_t i = 0; i < 32; ++i)
    {
        const uint16_t seg_sel =
            SEGMENT_SELECTOR_INDEX(GDT_INDEX_KERNEL_CODE) | SEGMENT_SELECTOR_RPL(0) | SEGMENT_SELECTOR_TI_GDT;
        idt_init_interrupt_gate(&idt.entries[i], (uint32_t)isr_dispatch_table[i], seg_sel, 0);
    }

    /* Setup user handler stubs. */
    for (uint32_t i = 32; i < 256; ++i)
    {
        const uint16_t seg_sel =
            SEGMENT_SELECTOR_INDEX(GDT_INDEX_KERNEL_CODE) | SEGMENT_SELECTOR_RPL(0) | SEGMENT_SELECTOR_TI_GDT;
        idt_init_interrupt_gate(&idt.entries[i], (uint32_t)isr_dispatch_table[i], seg_sel, 0);
    }
}

void idt_register_handler(uint8_t vector, idt_isr_t handler)
{
    ASSERT(vector >= 32);
    isr_user_handlers[vector] = handler;
}
