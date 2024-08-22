#include <stdint.h>

#include "term.h"
#include "multiboot.h"

#if !defined(__i386__)
#error "Must use i386-elf cross compiler!"
#endif

void print_multiboot_info(multiboot_info_t* mb_info)
{
    term_print("multiboot info:\n");
#define PRINT_VAR(var) term_print_var(#var, mb_info->var)
    PRINT_VAR(vbe_mode);
    PRINT_VAR(vbe_control_info);
    PRINT_VAR(vbe_mode_info);
    PRINT_VAR(framebuffer_addr);
    PRINT_VAR(framebuffer_type);
    PRINT_VAR(framebuffer_bpp);
    PRINT_VAR(framebuffer_width);
    PRINT_VAR(framebuffer_height);
    PRINT_VAR(framebuffer_pitch);
    PRINT_VAR(framebuffer_palette_addr);
    PRINT_VAR(framebuffer_palette_num_colors);
#undef PRINT_VAR
}

void kernel_main()
{
    uint32_t mb_magic;
    multiboot_info_t* mb_info;
    // https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#OutputOperands
    // %0 refers to the first output operand.
    // =r means the value resides in a register.
    __asm__("mov %%eax, %0" : "=r"(mb_magic));
    __asm__("mov %%ebx, %0" : "=r"(mb_info));

    term_init();

    term_print("chp-os kernel_main\n");
    term_print("Hello from kernel_main!\n");

    term_print_var("mb_magic", mb_magic);
    term_print_var("mb_info", (uint32_t)mb_info);

    term_print_colormap();

    print_multiboot_info(mb_info);
}
