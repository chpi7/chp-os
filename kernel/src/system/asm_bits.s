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

.macro generic_isr_dispatch id
isr_dispatch_\id:
    push %ebp
    mov %esp, %ebp
    # TODO: proper handling of error code, etc (see Vol3 6.13)
    pop %ebp
    iret
.endm

.macro isr_user_stubX id
isr_user_stub_\id:
    push %ebp
    mov %esp, %ebp
    mov $isr_user_handlers, %eax
    mov (\id*4)(%eax), %eax
    test %eax, %eax
    jz isr_usr_stub_no_handler_\id
    call *%eax
    isr_usr_stub_no_handler_\id:
    pop %ebp
    iret
.endm

.altmacro
.set i, 0
.rept 32
    generic_isr_dispatch %i
    .set i, i + 1
.endr

.set i, 32
.rept 256
    isr_user_stubX %i
    .set i, i + 1
.endr


.macro isr_insert_genericX number
    .long isr_dispatch_\number
.endm
.macro isr_insert_userX number
    .long isr_user_stub_\number
.endm

.section .data
.align 4
.global isr_dispatch_table
isr_dispatch_table:
    .set i, 0
    .rept 32
        isr_insert_genericX %i
        .set i, i + 1
    .endr
    .set i, 32
    .rept 224
        isr_insert_userX %i
        .set i, i + 1
    .endr

.global isr_user_handlers
isr_user_handlers:
    .skip 1024, 0
