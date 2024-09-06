#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "boot/multiboot.h"
#include "common/arrayutil.h"
#include "common/assert.h"
#include "common/commondefs.h"
#include "io/ps2/ps2.h"
#include "io/tty/tty.h"
#include "system/segment.h"
#include "system/interrupt.h"

#if !defined(__i386__)
#error "Must use i386-elf cross compiler!"
#endif

__attribute__((constructor)) void test_constructor(void)
{
    // printf("Running test-constructor\n");
}

void test_memcpy()
{
    bool passed = true;

    uint8_t src_array[256] = {};
    uint8_t dst_array[256] = {};

    /* basic memcpy without any overlap */

    for (size_t i = 0; i < 256; ++i)
    {
        src_array[i] = i;
    }

    memcpy(dst_array, src_array, sizeof(src_array));

    for (size_t i = 0; i < 256; ++i)
    {
        passed &= dst_array[i] == src_array[i];
    }

    memcpy(src_array, dst_array, sizeof(src_array));

    for (size_t i = 0; i < 256; ++i)
    {
        passed &= dst_array[i] == src_array[i];
    }

    if (passed)
    {
        printf("memcpy okay\n");
    }
    else
    {
        printf("memcpy failed\n");
    }
}

void init_data_array(uint8_t* data, size_t n)
{
    for (size_t i = 0; i < n; ++i)
    {
        data[i] = (uint8_t)i;
    }
}

void test_memmove()
{
    struct
    {
        size_t from, to, n;
    } testcases[] = {
        {.from = 0, .to = 10, .n = 10},  /* no overlap, src before dst */
        {.from = 5, .to = 10, .n = 10},  /* overlap, src before dst */
        {.from = 10, .to = 10, .n = 10}, /* overlap, src == dst */
        {.from = 15, .to = 10, .n = 10}, /* overlap, src after dst */
        {.from = 20, .to = 10, .n = 10}, /* no overlap, src after dst */
    };
    uint8_t data[256] = {};

    bool overall_passed = true;
    for (size_t testcase_idx = 0; testcase_idx < NELEMS(testcases); ++testcase_idx)
    {
        init_data_array(data, 256);

        const size_t from = testcases[testcase_idx].from;
        const size_t to = testcases[testcase_idx].to;
        const size_t n = testcases[testcase_idx].n;

        memmove((void*)&data[to], (void*)&data[from], n);

        bool passed = true;
        for (size_t i = 0; i < n; ++i)
        {
            passed &= data[to + i] == from + i;
        }

        overall_passed &= passed;

        ASSERT_MSG(passed, "memmove testcase %u failed", testcase_idx);
    }

    if (overall_passed)
    {
        printf("memmove okay\n");
    }
    else
    {
        printf("memmove failed\n");
    }
}

void print_multiboot_info(multiboot_info_t* mb_info)
{
    printf("multiboot info:\n");
#define PRINT_VAR(var) printf(#var ": 0x%x\n", mb_info->var)
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

static uint32_t mb_magic;
static multiboot_info_t* mb_info;

/* This just initializes the output (and maybe the heap or other things later) */
void kernel_early_main()
{
    // https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#OutputOperands
    // %0 refers to the first output operand.
    // =r means the value resides in a register.
    __asm__("mov %%eax, %0" : "=r"(mb_magic));
    __asm__("mov %%ebx, %0" : "=r"(mb_info));

    tty_init();

    const uint32_t expect_mb_magic = 0x2badb002;

    printf("[early main]: mb_magic okay: %s\n", BOOL_YES_NO(expect_mb_magic == mb_magic));
    // printf("mb_info @ 0x%p\n", mb_info);
    // print_multiboot_info(mb_info);
}

/* The main main kernel entrypoint */
void kernel_main()
{
    printf("chp-os kernel_main\n");

    printf("[self-test]: memutil\n");
    test_memcpy();
    test_memmove();

    gdt_init();
    gdt_flush();

    idt_init();

    tty_print_colormap();
    // tty_ascii_printable_table();

    ps2_init();
}
