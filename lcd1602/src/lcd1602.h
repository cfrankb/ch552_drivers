#pragma once

#include <stdint.h>

void i2clcd_init(const uint8_t num_lines, const uint8_t num_columns);
void i2clcd_display_on(void);
void i2clcd_display_off(void);
void i2clcd_backlight_on(void);
void i2clcd_backlight_off(void);
void i2clcd_clear(void);
void i2clcd_hide_cursor(void);
void i2clcd_blink_cursor_on(void);
void i2clcd_putstr(const char *s);