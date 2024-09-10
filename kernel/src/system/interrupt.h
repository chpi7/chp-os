#pragma once

#include <stdint.h>

#define IDT_MAX_ENTRIES (255)
typedef struct idt_descriptor_t
{
    uint16_t offset_0_15;
    uint16_t cs;
    uint16_t flags;
    uint16_t offset_16_31;
} __attribute__((packed)) idt_descriptor_t;
typedef struct idt_t
{
    idt_descriptor_t entries[IDT_MAX_ENTRIES];
} idt_t __attribute__((aligned(8)));
typedef struct idtr_t
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

typedef void (*idt_isr_t) ();

/**
 * Initialize the IDT.
 */
void idt_init();

/**
 * Flush the IDT (Execute LIDT).
 */
void idt_flush();

/**
 * Register a handler for the specified interrupt vector (>= 32)
 */
void idt_register_handler(uint8_t vector, idt_isr_t handler);
