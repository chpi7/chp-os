#include "term.h"

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
int term_col = 0;
int term_row = 0;
uint8_t term_color = 0x0F;  // BG: black, FG: white

void term_init()
{
    for (int col = 0; col < VGA_COLS; ++col)
    {
        for (int row = 0; row < VGA_ROWS; ++row)
        {
            /* VGA textmode buffer is (VGA_COLS * VGA_ROWS).
             * Each entry is a u16 of the form: BBBBFFFF CCCCCCCC.
             */
            const size_t index = (VGA_COLS * row) + col;
            vga_buffer[index] = ((uint16_t)term_color << 8) | ' ';
        }
    }
}

void term_col_fg(uint8_t c) { term_color = (term_color & 0xF0) | (c & 0x0F); }

void term_col_bg(uint8_t c) { term_color = (c & 0xF0) | (term_color & 0x0F); }

void term_putc(char c)
{
    switch (c)
    {
        case '\n':
            term_col = 0;
            ++term_row;
            break;
        default:
        {
            const size_t index = (VGA_COLS * term_row) + term_col;
            vga_buffer[index] = ((uint16_t)term_color << 8) | c;
            ++term_col;
            break;
        }
    }

    /* If past the end of a row, go to next row. */
    if (term_col >= VGA_COLS)
    {
        term_col = 0;
        ++term_row;
    }

    /* If past the last row, reset to top. */
    if (term_row >= VGA_ROWS)
    {
        term_col = 0;
        term_row = 0;
    }
}

void term_print(const char* str)
{
    for (size_t i = 0; str[i] != '\0'; ++i)
    {
        term_putc(str[i]);
    }
}

void term_put_i8_dec(uint8_t num)
{
    uint8_t div = 100;
    for (size_t i = 0; i < 3; ++i)
    {
        const uint8_t digit = num / div;
        num = num % div;
        div /= 10;
        if (num > 0 || 1)
        {
            term_putc('0' + digit);
        }
    }
}

void term_put_uint32(uint32_t num)
{
    term_print("0x");
    for (size_t i = 8; i > 0; --i)
    {
        const size_t offset = (i - 1) * 4;
        const uint8_t c = (num >> offset) & 0x0F;
        const uint8_t base_ascii = c < 10 ? '0' : ('a' - 10);
        term_putc(base_ascii + c);
    }
}

void term_print_var(const char* name, uint32_t num)
{
    term_print(name);
    term_print(" = ");
    term_put_uint32(num);
    term_putc('\n');
}

void term_print_colormap()
{
    term_print("--- Term Colors ---\n");
    term_print("Foregrounds:\n");
    for (uint8_t fg = 0; fg < 16; ++fg)
    {
        term_col_fg(fg);
        term_put_i8_dec(fg);
        term_putc(' ');
    }
    term_col_fg(0xF);
    term_putc('\n');

    term_print("Backgrounds:\n");
    for (uint8_t bg = 0; bg < 16; ++bg)
    {
        term_col_bg(bg);
        term_put_i8_dec(bg);
        term_putc(' ');
    }
    term_col_bg(0x0);
    term_putc('\n');
}
