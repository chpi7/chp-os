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
 * Print a null-terminated string.
 */
void term_print(const char* str);

/** 
 * Print an i8 formatted as decimal.
 */
void term_put_i8_dec(uint8_t num);
void term_put_uint32(uint32_t num);
void term_print_colormap();
void term_print_var(const char* name, uint32_t num);

