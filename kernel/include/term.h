#pragma once

#include <stddef.h>
#include <stdint.h>

#define VGA_COLS ((int)80)
#define VGA_ROWS ((int)25)

/** 
 * Initialize term module.
 */
void term_init();

/** 
 * Set foreground color to c.
 */
void term_col_fg(uint8_t c);

/** 
 * Set background color to c.
 */
void term_col_bg(uint8_t c);

/** 
 * Write one character at the current buffer position.
 */
void term_putc(char c);

/**
 * For printf support
 */
void _putchar(char character);

