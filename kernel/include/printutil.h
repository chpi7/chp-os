#include "term.h"
#include "printf.h"

void print_colormap()
{
    printf("--- Term Colors ---\n");
    printf("Foregrounds:\n");
    for (uint8_t fg = 0; fg < 16; ++fg)
    {
        term_col_fg(fg);
        printf(" %02u ", fg);
    }
    term_col_fg(0xF);
    term_putc('\n');

    printf("Backgrounds:\n");
    for (uint8_t bg = 0; bg < 16; ++bg)
    {
        if (bg >= 8)
        {
            /* Black fg for light bg colors. */
            term_col_fg(0);
        }
        term_col_bg(bg);
        printf(" %02u ", bg);
    }
    term_col_bg(0x0);
    term_col_fg(0xF);
    term_putc('\n');
}
