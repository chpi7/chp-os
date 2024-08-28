#include <stdbool.h>
#include <stdint.h>

#include "arrayutil.h"
#include "assert.h"
#include "memory.h"
#include "multiboot.h"
#include "printutil.h"

#if !defined(__i386__)
#error "Must use i386-elf cross compiler!"
#endif

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

    printf("chp-os kernel_main\n");
    printf("Hello from kernel_main!\n");
    printf("mb_magic = %x\n", mb_magic);
    printf("mb_info = %p\n", mb_info);

    printf("memutil self-test:\n");
    test_memcpy();
    test_memmove();
    printf("\n");

    print_colormap();

    print_multiboot_info(mb_info);
}
