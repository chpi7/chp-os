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

# CS: gdt[1] rpl 0                  1 << 3 == 0x8
# data segments: gdt[2] rpl 0       1 << 4 == 0x10
# layout [table index, table indicator (gdt == 0), rpl]
#        15         3            2                1  0
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

.global idtr_reload
idtr_reload:
    push %ebp
    mov %esp, %ebp
    pushf
    cli
    lidt (idtr)
    popf
    pop %ebp
    ret

.extern isr_0_31

.macro generic_isr_dispatch id=0
isr_dispatch_\id:
    # push %ebp
    # mov %esp, %ebp
    push $\id
    call isr_0_31
    # pop %ebp
    iret
.endm

generic_isr_dispatch 0
generic_isr_dispatch 1
generic_isr_dispatch 2
generic_isr_dispatch 3
generic_isr_dispatch 4
generic_isr_dispatch 5
generic_isr_dispatch 6
generic_isr_dispatch 7
generic_isr_dispatch 8
generic_isr_dispatch 9
generic_isr_dispatch 10
generic_isr_dispatch 11
generic_isr_dispatch 12
generic_isr_dispatch 13
generic_isr_dispatch 14
generic_isr_dispatch 15
generic_isr_dispatch 16
generic_isr_dispatch 17
generic_isr_dispatch 18
generic_isr_dispatch 19
generic_isr_dispatch 20
generic_isr_dispatch 21
generic_isr_dispatch 22
generic_isr_dispatch 23
generic_isr_dispatch 24
generic_isr_dispatch 25
generic_isr_dispatch 26
generic_isr_dispatch 27
generic_isr_dispatch 28
generic_isr_dispatch 29
generic_isr_dispatch 30
generic_isr_dispatch 31


.section .data
.align 8
.global isr_dispatch_table
isr_dispatch_table:
    .long (isr_dispatch_0)
    .long isr_dispatch_1
    .long isr_dispatch_2
    .long isr_dispatch_3
    .long isr_dispatch_4
    .long isr_dispatch_5
    .long isr_dispatch_6
    .long isr_dispatch_7
    .long isr_dispatch_8
    .long isr_dispatch_9
    .long isr_dispatch_10
    .long isr_dispatch_11
    .long isr_dispatch_12
    .long isr_dispatch_13
    .long isr_dispatch_14
    .long isr_dispatch_15
    .long isr_dispatch_16
    .long isr_dispatch_17
    .long isr_dispatch_18
    .long isr_dispatch_19
    .long isr_dispatch_20
    .long isr_dispatch_21
    .long isr_dispatch_22
    .long isr_dispatch_23
    .long isr_dispatch_24
    .long isr_dispatch_25
    .long isr_dispatch_26
    .long isr_dispatch_27
    .long isr_dispatch_28
    .long isr_dispatch_29
    .long isr_dispatch_30
    .long isr_dispatch_31


