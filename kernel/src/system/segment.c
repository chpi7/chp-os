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

void init_segment_descriptor(segment_descriptor* desc, uint32_t base, uint32_t limit, segment_data_type type,
                                             segment_descr_flags flags)
{
    uint32_t high_part = 0;
    high_part |= 0xFF000000 & base;
    high_part |= (0x00FF0000 & base) >> 16;
    high_part |= flags;  // flags are already shifted to the correct position inside the high part
    high_part |= 0x000F0000 & limit;
    high_part |= 0x00000F00 & (((uint32_t)(type)) << 8);
    uint32_t *high_ptr = (uint32_t*)(desc) + 1;
    *high_ptr = high_part;


    uint32_t low_part = 0;
    low_part |= limit & 0x0000FFFF;
    low_part |= 0xFFFF0000 & (base << 16);
    *((uint32_t*)desc) = low_part;

    LOG_DEBUG(SYS_LOG_MOD, "init seg desc as [%08x %08x]", *(((uint32_t*)desc) + 1), *(uint32_t*)desc);
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
    const segment_descr_flags code_data_4kblim_32bit_flags = SEGMENT_DESCR_SIZE(1) | SEGMENT_DESCR_PRES(1) | 
                                     SEGMENT_DESCR_DTYP(1) | SEGMENT_DESCR_GRAN(1);
    init_segment_descriptor(&gdt.descriptors[1], 0, limit_4kb, SEGMENT_CODE_TYPE_EXRD, code_data_4kblim_32bit_flags);
    init_segment_descriptor(&gdt.descriptors[2], 0, limit_4kb, SEGMENT_DATA_TYPE_RDWR, code_data_4kblim_32bit_flags);

    gdt_limit = sizeof(segment_descriptor) * 3 - 1; // segment descriptor is 8 byte

    // LOG_DEBUG(SYS_LOG_MOD, "GDT @ 0x%p, limit = 0x%x (%u)", (void*)&gdt, gdt_limit, gdt_limit);
    LOG_DEBUG(SYS_LOG_MOD, "Init GDT done (basic flat model)");
}
