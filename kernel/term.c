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

void term_col_bg(uint8_t c) { term_color = ((c << 4) & 0xF0) | (term_color & 0x0F); }

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

void _putchar(char character) { term_putc(character); }
