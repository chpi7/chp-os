#pragma once

#include <stdint.h>

typedef struct segment_descriptor
{
    uint16_t seg_lim_0_15 : 16;
    uint16_t base_addr_0_15 : 16;
    uint8_t base_addr_16_23 : 8;
    uint8_t type : 4;
    uint8_t dtype : 1;
    uint8_t dpl : 2;
    uint8_t present : 1;
    uint8_t seg_lim_16_19 : 4;
    uint8_t avl : 1;
    uint8_t l : 1;
    uint8_t db : 1;
    uint8_t g : 1;
    uint8_t base_24_31 : 8;
} __attribute__((packed)) segment_descriptor;

typedef uint32_t segment_descr_flags;

#define SEGMENT_DESCR_ENCODE_FLAG(v, offset) (((segment_descr_flags)(v & 0x1)) << offset)
#define SEGMENT_DESCR_GRAN(v) SEGMENT_DESCR_ENCODE_FLAG(v, 23)
#define SEGMENT_DESCR_SIZE(v) SEGMENT_DESCR_ENCODE_FLAG(v, 22)
#define SEGMENT_DESCR_64BI(v) SEGMENT_DESCR_ENCODE_FLAG(v, 21)
#define SEGMENT_DESCR_AVAI(v) SEGMENT_DESCR_ENCODE_FLAG(v, 20)
#define SEGMENT_DESCR_PRES(v) SEGMENT_DESCR_ENCODE_FLAG(v, 15)
#define SEGMENT_DESCR_PRIV(t) (((segment_descr_flags)(t & 0x3)) << 13)
#define SEGMENT_DESCR_DTYP(v) SEGMENT_DESCR_ENCODE_FLAG(v, 12)

#define SEGMENT_GDT_NUM_ENTRIES (8)  // can be up to to 2**13 == 8192 entries large

typedef struct gdt_t
{
    segment_descriptor descriptors[SEGMENT_GDT_NUM_ENTRIES];
} gdt_t __attribute__((aligned(8)));

/**
 * As defined in:
 * 3.4.5.1 Code- and Data-Segment Descriptor Types
 */
typedef enum segment_data_type
{
    SEGMENT_DATA_TYPE_RD = 0x00,         // Read-Only
    SEGMENT_DATA_TYPE_RDA = 0x01,        // Read-Only, accessed
    SEGMENT_DATA_TYPE_RDWR = 0x02,       // Read/Write
    SEGMENT_DATA_TYPE_RDWRA = 0x03,      // Read/Write, accessed
    SEGMENT_DATA_TYPE_RDEXPD = 0x04,     // Read-Only, expand-down
    SEGMENT_DATA_TYPE_RDEXPDA = 0x05,    // Read-Only, expand-down, accessed
    SEGMENT_DATA_TYPE_RDWREXPD = 0x06,   // Read/Write, expand-down
    SEGMENT_DATA_TYPE_RDWREXPDA = 0x07,  // Read/Write, expand-down, accessed
    SEGMENT_CODE_TYPE_EX = 0x08,         // Execute-Only
    SEGMENT_CODE_TYPE_EXA = 0x09,        // Execute-Only, accessed
    SEGMENT_CODE_TYPE_EXRD = 0x0A,       // Execute/Read
    SEGMENT_CODE_TYPE_EXRDA = 0x0B,      // Execute/Read, accessed
    SEGMENT_CODE_TYPE_EXC = 0x0C,        // Execute-Only, conforming
    SEGMENT_CODE_TYPE_EXCA = 0x0D,       // Execute-Only, conforming, accessed
    SEGMENT_CODE_TYPE_EXRDC = 0x0E,      // Execute/Read, conforming
    SEGMENT_CODE_TYPE_EXRDCA = 0x0F,     // Execute/Read, conforming, accessed
} segment_data_type;

/**
 * 3.5 SYSTEM DESCRIPTOR TYPES
 */
typedef enum segment_sys_gate_type
{
    SEGMENT_TSS_AVAIL_16 = 1,
    SEGMENT_LDT = 2,
    SEGMENT_TSS_BUSY_16 = 3,
    SEGMENT_GATE_CALL_16 = 4,
    SEGMENT_GATE_TASK = 5,
    SEGMENT_GATE_INT_16 = 6,
    SEGMENT_GATE_TRAP_16 = 7,
    SEGMENT_TSS_AVAIL_32 = 9,
    SEGMENT_TSS_BUSY_32 = 11,
    SEGMENT_GATE_CALL_32 = 12,
    SEGMENT_GATE_INT_32 = 14,
    SEGMENT_GATE_TRAP_32 = 15,
} segment_sys_gate_type;

/**
 * Create descriptor according to figure 3.4.5 Segment Descriptors
 */
void init_segment_descriptor(segment_descriptor* desc, uint32_t base, uint32_t limit, segment_data_type type,
                             segment_descr_flags flags);

/**
 * Flush the GDT (Execute LGDT, reload all segment registers).
 */
void gdt_flush();

/**
 * Initialize the GDT (for basic flat model).
 */
void gdt_init();
