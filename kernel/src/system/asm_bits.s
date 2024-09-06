.section .data
gdtr_limit: .word 8
gdtr_base:  .long 1


.section .text

    .global segment_set_gdtr_asm_prot
    segment_set_gdtr_asm_prot:
        push %ebx
        mov 8(%esp), %eax # base
        mov $gdtr_base, %ebx
        mov %eax, (%ebx)
        mov 12(%esp), %eax # limit
        mov $gdtr_limit, %ebx
        movw %ax, (%ebx)
        pop %ebx
        lgdt (gdtr_limit)
        ret

# reload segments for flat model with code at offset 8, data offset 16
    .global segment_reload_segments
    segment_reload_segments:
        ljmp $0x08, $reload_cs
        # pushw $0x08
        # pushl $reload_cs
    reload_cs:
        mov $0x10, %ax
        mov %ax, %ss
        mov %ax, %ss
        mov %ax, %ds
        mov %ax, %es
        mov %ax, %fs
        mov %ax, %gs

        ret
