#include "term.h"

#include "memory.h"

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
int term_col = 0;
int term_row = 0;
uint8_t term_color = 0x0F;  // BG: black, FG: white

void term_init()
{
    term_clear();
}

void term_col_fg(uint8_t c) { term_color = (term_color & 0xF0) | (c & 0x0F); }

void term_col_bg(uint8_t c) { term_color = ((c << 4) & 0xF0) | (term_color & 0x0F); }

static inline uint16_t get_blank()
{
    return ((uint16_t)term_color << 8) | ' ';
}

/**
 * @brief This scrolls the vga buffer up by one row
 */
void term_scroll_up() 
{
    const size_t move_size = VGA_COLS * (VGA_ROWS - 1) * sizeof(uint16_t);
    memmove((void*)(vga_buffer), (void*)(vga_buffer + VGA_COLS), move_size);

    for (int col = 0; col < VGA_COLS; ++col)
    {
        const size_t index = (VGA_COLS * (VGA_ROWS - 1)) + col;
        vga_buffer[index] = get_blank();
    }
}

void term_clear()
{
    term_col = 0;
    term_row = 0;

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

    /* If past the last row, scroll up one row. */
    if (term_row >= VGA_ROWS)
    {
        term_col = 0;
        --term_row;
        term_scroll_up();
    }
}

void _putchar(char character) { term_putc(character); }
