#include "ps2.h"

#include <stdbool.h>
#include <stdint.h>

#include "commondefs.h"
#include "printf.h"

#define IO_PORT_PS2_DATA (0x60)
#define IO_PORT_PS2_STATUS (0x64)
#define IO_PORT_PS2_COMMAND (0x64)

#define IO_COMMAND_PS2_DISABLE_1 (0xAD)
#define IO_COMMAND_PS2_DISABLE_2 (0xA7)
#define IO_COMMAND_PS2_ENABLE_1 (0xAE)
#define IO_COMMAND_PS2_ENABLE_2 (0xA8)
#define IO_COMMAND_PS2_TEST_CONTROLLER (0xAA)
#define IO_COMMAND_PS2_TEST_PORT_1 (0xAB)
#define IO_COMMAND_PS2_TEST_PORT_2 (0xA9)
#define IO_COMMAND_PS2_READ_CONFIG (0x20)
#define IO_COMMAND_PS2_WRITE_CONFIG (0x60)
#define IO_COMMAND_PS2_WRITE_PORT2_INPUT (0xD4)

#define PS2_CONFIG_INTERRUPT_1_BIT ((uint8_t)(1 << 0))
#define PS2_CONFIG_INTERRUPT_2_BIT ((uint8_t)(1 << 1))
#define PS2_CONFIG_SYSTEM_FLAG_BIT ((uint8_t)(1 << 2))
#define PS2_CONFIG_CLOCK_1_BIT ((uint8_t)(1 << 4))
#define PS2_CONFIG_CLOCK_2_BIT ((uint8_t)(1 << 5))
#define PS2_CONFIG_TRANSLATE_1_BIT ((uint8_t)(1 << 5))

#define PS2_LOG(msg, ...) printf("ps/2: " msg "\n", __VA_ARGS__)
#define PS2_LOG_DEV(dev, msg, ...) printf("ps/2 (dev %u): " msg "\n", dev->is_port_1 ? 1 : 2, __VA_ARGS__)

typedef enum ps2_device_type
{
    /** not yet handled. */
    ps2_device_type_unknown,
    /** Some old keyboard */
    ps2_device_type_none, 
    /** Standard PS/2 mouse */
    ps2_device_type_00,
    /** Mouse with scroll wheel */
    ps2_device_type_03,
} ps2_device_type;

typedef enum ps2_device_status
{
    ps2_device_status_not_connected,
    ps2_device_status_self_check_fail,
    ps2_device_status_no_device_id,
    ps2_device_status_okay,
} ps2_device_status;

typedef struct ps2_device
{
    ps2_device_type type;
    ps2_device_status status;
    bool is_port_1;
} ps2_device;

typedef enum ps2_error
{
    ps2_error_none,
    ps2_error_timeout,
} ps2_error;

/* Whether two channels are supported. */
static bool ps2_is_dual_channel = false;
/* Whether port 1 has passed the self test. */
static bool ps2_is_port1_valid = false;
/* Whether port 2 has passed the self test. */
static bool ps2_is_port2_valid = false;

static ps2_device ps2_device1 = {.is_port_1 = true};
static ps2_device ps2_device2 = {.is_port_1 = false};

static void ps2_write_data(uint8_t data)
{
    /* no single line */
    __asm__ volatile("movb %0, %%al; outb %%al, $0x60" ::"r"(data) : "al");
}

static uint8_t ps2_read_data()
{
    register uint8_t result __asm__("al");
    __asm__ volatile("inb $0x60, %0" : "=r"(result));
    return result;
}

static void ps2_write_command(uint8_t cmd)
{
    /* no single line */
    __asm__ volatile("movb %0, %%al; outb %%al, $0x64" ::"r"(cmd) : "al");
}

static uint8_t ps2_read_status()
{
    register uint8_t result __asm__("al");
    __asm__ volatile("inb $0x64, %0" : "=r"(result));
    /* Without specific register for result: */
    /* __asm__ volatile ("inb $0x60, %%al; mov %%al, %0" : "=r"(result) : : "al"); */
    return result;
}

/* Must be true before writing port 0x60 or 0x64. */
static bool ps2_input_ready()
{
    /* check if bit 1 is clear */
    uint8_t status_data = ps2_read_status();
    return ~status_data & 0x02;
}

/* Wait until 0x60 or 0x64 is ready for writing. */
static ps2_error ps2_wait_input_ready()
{
    int timeout = 100000000;
    while (!ps2_input_ready() && timeout-- > 0)
        ;
    return timeout <= 0 ? ps2_error_timeout : ps2_error_none;
}

/* Must be true before reading port 0x60. */
static bool ps2_output_ready()
{
    /* check if bit 0 is set */
    uint8_t status_data = ps2_read_status();
    // printf("status_data = %x\n", status_data);
    return status_data & 0x01;
}

/* Wait until 0x60 is ready for reading. */
static ps2_error ps2_wait_output_ready()
{
    int timeout = 10000;
    while (!ps2_output_ready() && timeout-- > 0)
        ;
    return timeout <= 0 ? ps2_error_timeout : ps2_error_none;
}

static void ps2_check_dual_channel()
{
    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_ENABLE_2);
    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
    ps2_wait_output_ready();
    uint8_t config = ps2_read_data();
    /* Check if bit 5 is clear. */
    ps2_is_dual_channel = ~config & PS2_CONFIG_CLOCK_2_BIT;

    printf("ps/2: dual channel: %s\n", BOOL_YES_NO(ps2_is_dual_channel));

    if (ps2_is_dual_channel)
    {
        ps2_wait_input_ready();
        ps2_write_command(IO_COMMAND_PS2_DISABLE_2);

        ps2_wait_input_ready();
        ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
        ps2_wait_output_ready();
        config = ps2_read_data();

        /* clear bits to enable */
        config &= ~(PS2_CONFIG_INTERRUPT_2_BIT | PS2_CONFIG_CLOCK_2_BIT);

        ps2_wait_input_ready();
        ps2_write_command(IO_COMMAND_PS2_WRITE_CONFIG);
        ps2_wait_input_ready();
        ps2_write_data(config);
    }
}

static void ps2_ports_self_check()
{
    const uint8_t cmds[] = {IO_COMMAND_PS2_TEST_PORT_1, IO_COMMAND_PS2_TEST_PORT_2};
    bool *okay_flags[] = {&ps2_is_port1_valid, &ps2_is_port2_valid};
    for (uint8_t port_idx = 0; port_idx < 2; ++port_idx)
    {
        ps2_wait_input_ready();
        ps2_write_command(cmds[port_idx]);
        ps2_wait_output_ready();
        uint8_t response = ps2_read_data();
        bool port_okay = response == 0x00;
        *okay_flags[port_idx] = port_okay;
        printf("ps/2: port %u self-check okay: %s (response=0x%x)\n", port_idx + 1, BOOL_YES_NO(port_okay), response);
    }
}

static void ps2_enable_ports()
{
    if (ps2_is_port1_valid)
    {
        ps2_wait_input_ready();
        ps2_write_command(IO_COMMAND_PS2_ENABLE_1);
    }

    if (ps2_is_port2_valid)
    {
        ps2_wait_input_ready();
        ps2_write_command(IO_COMMAND_PS2_ENABLE_2);
    }

    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
    ps2_wait_output_ready();
    uint8_t config = ps2_read_data();

    /* Enable irq on valid ports */
    if (ps2_is_port1_valid)
    {
        config |= PS2_CONFIG_INTERRUPT_1_BIT;
    }
    if (ps2_is_port2_valid)
    {
        config |= PS2_CONFIG_INTERRUPT_2_BIT;
    }

    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_WRITE_CONFIG);
    ps2_wait_input_ready();
    ps2_write_data(config);

    printf("ps/2: ports enabled (1: %s, 2: %s)\n", BOOL_YES_NO(ps2_is_port1_valid), BOOL_YES_NO(ps2_is_port2_valid));
}

static ps2_error ps2_device_write_data(ps2_device *device, uint8_t data)
{
    if (!device->is_port_1)
    {
        ps2_error error = ps2_wait_input_ready();
        if (error != ps2_error_none) return error;

        ps2_write_command(IO_COMMAND_PS2_WRITE_PORT2_INPUT);
    }

    ps2_error error = ps2_wait_input_ready();
    if (error != ps2_error_none) return error;

    ps2_write_data(data);

    return ps2_error_none;
}

static ps2_error ps2_device_read_data(ps2_device *device, uint8_t *data)
{
    ps2_error error = ps2_wait_output_ready();
    if (error != ps2_error_none) return error;

    *data = ps2_read_data();

    return ps2_error_none;
}

static void ps2_reset_device(ps2_device *device)
{
    device->status = ps2_device_status_not_connected;

    /* At first reset, then wait for the response byte(s). */
    ps2_device_write_data(device, 0xFF);

    uint16_t response = 0;
    uint8_t out = 0;
    ps2_error error = ps2_device_read_data(device, &out);
    /* If we don't get a reponse, nothing is connected. */
    if (ps2_error_timeout == error)
    {
        PS2_LOG_DEV(device, "not connected", "");
        device->status = ps2_device_status_not_connected;
        return;
    }

    response = out;

    /* If the response is FC, the self-check failed. */
    if (0xFC == response)
    {
        PS2_LOG_DEV(device, "self-check fail 1", "");
        device->status = ps2_device_status_self_check_fail;
        return;
    }

    /* For a successfull self-check we expect a second byte, if we get nothing, assume failed self-check. */
    error = ps2_device_read_data(device, &out);
    if (ps2_error_timeout == error)
    {
        PS2_LOG_DEV(device, "self-check fail 2", "");
        device->status = ps2_device_status_self_check_fail;
        return;
    }
    response |= out << 8;

    /* Order of response bytes can change. */
    bool reset_okay = (response == 0xAAFA || response == 0xFAAA);
    if (!reset_okay)
    {
        PS2_LOG_DEV(device, "self-check fail 3", "");
        device->status = ps2_device_status_self_check_fail;
        return;
    }

    /* after a successful self-check, the device id (0, 1 or 2 byte) is sent as well. */
    uint8_t device_id_1 = 0;
    uint8_t device_id_2 = 0;
    error = ps2_device_read_data(device, &device_id_1);
    if (ps2_error_timeout != error)
    {
        /* If first byte was received, try to receive a second one. */
        error = ps2_device_read_data(device, &device_id_2);
    }
    device->status = ps2_device_status_okay;
    uint16_t device_id = ((uint16_t)device_id_2) << 8 | device_id_1;

    switch (device_id)
    {
        case 0x00:
            device->type = ps2_device_type_00;
            break;
        default:
            PS2_LOG("WARN: unknown device id 0x%x", device_id);
            device->type = ps2_device_type_unknown;
    }


    PS2_LOG_DEV(device, "initialized with type %u", device->type);
}

static void ps2_reset_devices()
{
    ps2_reset_device(&ps2_device1);
    ps2_reset_device(&ps2_device2);
}

void ps2_init()
{
    /* TODO: use ACPI to check if PS2 is even there. For qemu we know it is. */

    /* -- Disable both ports so that they don't interfere with initialization. */
    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_DISABLE_1);
    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_DISABLE_2);

    /* -- Flush output buffer */
    ps2_read_data();

    /* -- Update config -- */
    ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
    ps2_wait_output_ready();
    uint8_t config = ps2_read_data();
    config &= ~(PS2_CONFIG_TRANSLATE_1_BIT | PS2_CONFIG_CLOCK_1_BIT | PS2_CONFIG_INTERRUPT_1_BIT);
    ps2_write_command(IO_COMMAND_PS2_WRITE_CONFIG);
    ps2_wait_input_ready();
    ps2_write_data(config);

    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_READ_CONFIG);
    ps2_wait_output_ready();
    uint8_t config_readback = ps2_read_data();

    /* -- Perform self-check */
    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_TEST_CONTROLLER);

    ps2_wait_output_ready();
    const uint8_t self_check_status = ps2_read_data();
    printf("ps/2: controller self-check passed: %s (response=0x%x)\n", BOOL_YES_NO(self_check_status == 0x55),
           self_check_status);

    if (0x55 != self_check_status)
    {
        printf("ps/2: failed to initialize\n");
        return;
    }

    /* -- restore config after self-check -- */
    ps2_wait_input_ready();
    ps2_write_command(IO_COMMAND_PS2_WRITE_CONFIG);
    ps2_wait_input_ready();
    ps2_write_data(config);

    /* -- perform config and interface checks -- */
    ps2_check_dual_channel();
    ps2_ports_self_check();

    if (!ps2_is_port1_valid && !ps2_is_port2_valid)
    {
        printf("ps/2: failed to initialize\n");
        return;
    }

    ps2_enable_ports();
    ps2_reset_devices();

    printf("ps/2: init done\n");
}
