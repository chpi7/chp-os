#include "system/segment.h"

#include "common/assert.h"
#include "log/log.h"

#define SYS_LOG_MOD "sys"

/* The GDT. */
gdt_t gdt;

/* End must point to the last valid byte, not the first invalid byte past it. */
uint16_t gdt_limit = 0;

/* Set gdtr from protected mode right after coming in from multiboot.
 * Segmented memory but with one segment with base = 0, limit = fffff..
 */
extern void segment_set_gdtr_asm_prot(uint32_t base, uint16_t limit);

extern void segment_reload_segments();

void init_segment_descriptor(segment_descriptor *desc, uint32_t base, uint32_t limit, segment_data_type type,
                             segment_descr_flags flags)
{
    uint32_t high_part = 0;
    high_part |= 0xFF000000 & base;
    high_part |= (0x00FF0000 & base) >> 16;
    high_part |= flags;  // flags are already shifted to the correct position inside the high part
    high_part |= 0x000F0000 & limit;
    high_part |= 0x00000F00 & (((uint32_t)(type)) << 8);
    uint32_t *high_ptr = (uint32_t *)(desc) + 1;
    *high_ptr = high_part;

    uint32_t low_part = 0;
    low_part |= limit & 0x0000FFFF;
    low_part |= 0xFFFF0000 & (base << 16);
    *((uint32_t *)desc) = low_part;

    LOG_DEBUG(SYS_LOG_MOD, "init seg desc as [%08x %08x]", *(((uint32_t *)desc) + 1), *(uint32_t *)desc);
}

void gdt_flush()
{
    /* This can be used right after multiboot has booted our kernel. */
    segment_set_gdtr_asm_prot((uint32_t)&gdt, gdt_limit);
    segment_reload_segments();
    LOG_DEBUG(SYS_LOG_MOD, "Flushed GDT, reloaded segment regs");
}

void gdt_init()
{
    init_segment_descriptor(&gdt.descriptors[0], 0, 0, 0, 0);

    /* -- setup flat memory model, one code one read/write segment -- */

    const uint32_t limit_4kb = 0xfffff;
    const segment_descr_flags flags =
        SEGMENT_DESCR_SIZE(1) | SEGMENT_DESCR_PRES(1) | SEGMENT_DESCR_DTYP(1) | SEGMENT_DESCR_GRAN(1);
    init_segment_descriptor(&gdt.descriptors[GDT_INDEX_KERNEL_CODE], 0, limit_4kb, SEGMENT_CODE_TYPE_EXRD, flags);
    init_segment_descriptor(&gdt.descriptors[GDT_INDEX_KERNEL_DATA], 0, limit_4kb, SEGMENT_DATA_TYPE_RDWR, flags);

    gdt_limit = sizeof(segment_descriptor) * 3 - 1;  // segment descriptor is 8 byte

    // LOG_DEBUG(SYS_LOG_MOD, "GDT @ 0x%p, limit = 0x%x (%u)", (void*)&gdt, gdt_limit, gdt_limit);
    LOG_DEBUG(SYS_LOG_MOD, "Init GDT done (basic flat model)");
}

idtr_t idtr;
idt_t idt;

/* Not used yet.
typedef void (*interrupt_handler_t) (uint8_t);
interrupt_handler_t _interrupt_handler_table[SEGMENT_IDT_MAX_ENTRIES] = { NULL };
*/

extern uint32_t isr_dispatch_table[32];

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

void idt_init()
{
    /* Setup system handlers. */
    for (uint32_t i = 0; i < 32; ++i)
    {
        const uint16_t seg_sel =
            SEGMENT_SELECTOR_INDEX(GDT_INDEX_KERNEL_CODE) | SEGMENT_SELECTOR_RPL(0) | SEGMENT_SELECTOR_TI_GDT;
        idt_init_interrupt_gate(&idt.entries[i], (uint32_t)isr_dispatch_table[i], seg_sel, 0);
    }
}

extern void idtr_reload();

void idt_flush()
{
    idtr.limit = (sizeof(idt_descriptor_t) * 33) - 1;
    idtr.base = (uint32_t)&idt.entries[0];

    idtr_reload();

    LOG_DEBUG(SYS_LOG_MOD, "Flushed IDT");
}
