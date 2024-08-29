#include <stdbool.h>
#include <stdint.h>

#include "printf.h"

#define IO_PORT_PS2_DATA (0x60)
#define IO_PORT_PS2_STATUS (0x64)
#define IO_PORT_PS2_COMMAND (0x64)

#define IO_COMMAND_PS2_DISABLE_1 (0xAD)
#define IO_COMMAND_PS2_DISABLE_2 (0xA7)
#define IO_COMMAND_PS2_TEST_CONTROLLER (0xAA)
#define IO_COMMAND_PS2_READ_CONFIG (0x20)
#define IO_COMMAND_PS2_WRITE_CONFIG (0x60)

typedef struct ps2_status_register_t
{
    uint8_t output_buffer_status : 1;
    uint8_t input_buffer_status : 1;
    uint8_t system_flag : 1;
    uint8_t command_data : 1;
    uint8_t _unknown_0 : 1;
    uint8_t _unknown_1 : 1;
    uint8_t timeout_error : 1;
    uint8_t parity_error : 1;
} ps2_status_register;

void ps2_write_data(uint8_t data)
{
    /* no single line */
    __asm__ volatile("movb %0, %%al; outb %%al, $0x60" ::"r"(data) : "al");
}

uint8_t ps2_read_data()
{
    register uint8_t result __asm__("al");
    __asm__ volatile("inb $0x60, %0" : "=r"(result));
    return result;
}

void ps2_write_command(uint8_t cmd)
{
    /* no single line */
    __asm__ volatile("movb %0, %%al; outb %%al, $0x64" ::"r"(cmd) : "al");
}

uint8_t ps2_read_status()
{
    register uint8_t result __asm__("al");
    __asm__ volatile("inb $0x64, %0" : "=r"(result));
    /* Without specific register for result: */
    /* __asm__ volatile ("inb $0x60, %%al; mov %%al, %0" : "=r"(result) : : "al"); */
    return result;
}

bool ps2_input_ready()
{
    /* check if bit 1 is clear */
    uint8_t status_data = ps2_read_status();
    return ~status_data & 0x02;
}

void ps2_wait_input_ready()
{
    // printf("ps2: wait for input ready\n");
    while (!ps2_input_ready())
        ;
    // printf("ps2: input is ready\n");
}

bool ps2_output_ready()
{
    /* check if bit 0 is set */
    uint8_t status_data = ps2_read_status();
    // printf("status_data = %x\n", status_data);
    return status_data & 0x01;
}

void ps2_wait_output_ready()
{
    // printf("ps2: wait for output ready\n");
    while (!ps2_output_ready())
        ;
    /* TODO: add some kind of timeout here? */
    // printf("ps2: output is ready\n");
}

void test_ps2_port()
{
    /* TODO: use ACPI to check if PS2 is even there. For qemu we know it is. */

    /* -- Disable both ports so that they don't interfere with initialization. */
    ps2_write_command(IO_COMMAND_PS2_DISABLE_1);
    ps2_write_command(IO_COMMAND_PS2_DISABLE_2);

    /* -- Flush output buffer */
    ps2_read_data();

    /* -- Update config to use polling on port 1 */
    ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
    ps2_wait_output_ready();
    uint8_t config = ps2_read_data();
    printf("ps/2: controller config = %x\n", config);

    /* clear bits
     * 0: disable irq port 1
     * 4: enable clock port 1
     * 6: disable translation port 1. */
    config &= ~((1 << 6) | (1 << 4) | (1));

    ps2_write_command(IO_COMMAND_PS2_WRITE_CONFIG);
    ps2_wait_input_ready();
    ps2_write_data(config);

    ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
    ps2_wait_output_ready();
    uint8_t config_readback = ps2_read_data();
    printf("ps/2: config readback okay: %s\n", config == config_readback ? "yes" : "no");

    /* -- Perform self-check */
    ps2_write_command(IO_COMMAND_PS2_TEST_CONTROLLER);
    ps2_wait_output_ready();

    const uint8_t self_check_status = ps2_read_data();
    printf("ps/2: self-check passed: %s\n", self_check_status == 0x55 ? "yes" : "no");
}
