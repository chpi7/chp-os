// Name of the kernel entrypoint
.extern kernel_main

// Global entrypoint for the linker to use as the entrypoint
.global start

// GRUB multiboot "MB" header
.set MB_MAGIC, 0x1BADB002
// 0: load modules on page boundaries, 1: provide memory map
// 2: provide video mode table
.set MB_FLAGS, (1) | (1 << 1)
// Checksum: magic + flags + checksum must equal 0 mod 2^32
.set MB_CHECKSUM, (0 - (MB_MAGIC + MB_FLAGS))

.section .multiboot
    // align the following data to 4 bytes
    .align 4
    .long MB_MAGIC
    .long MB_FLAGS
    .long MB_CHECKSUM

.section .bss
    .align 16
    stack_bottom:
        .skip 4096
    stack_top:

.section .text
    start:
        // -- Set up environment for C
        // This only requires the stack to work
        // Thus, set up the stack pointer (stack grows downwards)
        mov $stack_top, %esp

        call kernel_main

        // In case this returns, which it should not really do, hang
        hang:
            // Disable interrupts
            cli 
            hlt
            jmp hang

