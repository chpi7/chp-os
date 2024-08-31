/* TTY Implementation using vga textmode. */

#include "io/tty/tty.h"

#include <string.h>

#define VGA_COLS ((int)80)
#define VGA_ROWS ((int)25)

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
int vga_col = 0;
int vga_row = 0;
uint8_t tty_color = 0x0F;  // BG: black, FG: white

static inline uint16_t get_blank() { return ((uint16_t)tty_color << 8) | ' '; }

static void tty_clear()
{
    vga_col = 0;
    vga_row = 0;

    for (int col = 0; col < VGA_COLS; ++col)
    {
        for (int row = 0; row < VGA_ROWS; ++row)
        {
            /* VGA textmode buffer is (VGA_COLS * VGA_ROWS).
             * Each entry is a u16 of the form: BBBBFFFF CCCCCCCC.
             */
            const size_t index = (VGA_COLS * row) + col;
            vga_buffer[index] = get_blank();
        }
    }
}
/**
 * @brief This scrolls the vga buffer up by one row
 */
static void tty_scroll_up()
{
    const size_t move_size = VGA_COLS * (VGA_ROWS - 1) * sizeof(uint16_t);
    memmove((void*)(vga_buffer), (void*)(vga_buffer + VGA_COLS), move_size);

    for (int col = 0; col < VGA_COLS; ++col)
    {
        const size_t index = (VGA_COLS * (VGA_ROWS - 1)) + col;
        vga_buffer[index] = get_blank();
    }
}

void tty_init() { tty_clear(); }

void tty_putc(char c)
{
    switch (c)
    {
        case '\n':
            vga_col = 0;
            ++vga_row;
            break;
        default:
        {
            const size_t index = (VGA_COLS * vga_row) + vga_col;
            vga_buffer[index] = ((uint16_t)tty_color << 8) | c;
            ++vga_col;
            break;
        }
    }

    /* If past the end of a row, go to next row. */
    if (vga_col >= VGA_COLS)
    {
        vga_col = 0;
        ++vga_row;
    }

    /* If past the last row, scroll up one row. */
    if (vga_row >= VGA_ROWS)
    {
        vga_col = 0;
        --vga_row;
        tty_scroll_up();
    }
}

void tty_set_fg(uint8_t c) { tty_color = (tty_color & 0xF0) | (c & 0x0F); }

void tty_set_bg(uint8_t c) { tty_color = ((c << 4) & 0xF0) | (tty_color & 0x0F); }

/* Output a null terminated string. */
void tty_puts(const char* string)
{
    while (*string != '\0')
    {
        tty_putc(*string);
        ++string;
    }
}

/* Print a number between 0 and 15 incl. */
static void output_num(uint8_t n)
{
    tty_putc(' ');
    if (n < 10)
    {
        tty_putc('0');
        tty_putc('0' + n);
    }
    else
    {
        tty_putc('1');
        tty_putc('0' + (n - 10));
    }
    tty_putc(' ');
}

void tty_print_colormap()
{
    const size_t row_len = 16 * 2;
    tty_puts("--- Term Colors ---\n");
    for (uint8_t fg = 0; fg < 16; ++fg)
    {
        tty_set_fg(fg);
        output_num(fg);
    }
    tty_set_fg(0xF);
    tty_putc('\n');

    for (uint8_t bg = 0; bg < 16; ++bg)
    {
        if (bg >= 8)
        {
            /* Black fg for light bg colors. */
            tty_set_fg(0);
        }
        tty_set_bg(bg);
        output_num(bg);
    }
    tty_set_bg(0x0);
    tty_set_fg(0xF);
    tty_putc('\n');
}

void tty_ascii_printable_table()
{
    for (uint16_t c = 0; c < 256; ++c)
    {
        if (c % 32 == 0 && c > 0)
        {
            tty_putc('\n');
        }
        if (c % 32 != 0)
        {
            tty_putc(' ');
        }
        if (c == 0x00 || c == 0x20)
        {
            tty_putc(' ');
        }
        else
        {
            tty_putc(c);
        }
    }
    tty_putc('\n');
}

void _putchar(char character) { tty_putc(character); }
