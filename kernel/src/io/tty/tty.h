#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/** 
 * Initialize tty.
 */
void tty_init();

void tty_putc(char c);

void tty_set_fg(uint8_t c);
void tty_set_bg(uint8_t c);

void tty_print_colormap();
void tty_ascii_printable_table();
