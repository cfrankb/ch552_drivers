// ===================================================================================
// Project:   TM1637 7-digit driver for CH551, CH552, CH554
// Version:   v1.1
// Year:      2024
// Author:    Francois Blanchette
// Github:    https://github.com/cfrankb
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// This code implements an interface for the standard
// TM1637 7-digit module.
//
// References:
// -----------
// - Wagiminator: https://github.com/wagiminator
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - TinyTerminal: https://github.com/wagiminator/ATtiny85-TinyTerminal
//
// Wiring:
// -------
//
// CLK < -- > P33
// DIO < -- > P34
//
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <stdio.h>
#include "src/config.h" // user configurations
#include "src/system.h" // system functions
#include "src/gpio.h"   // for GPIO
#include "src/delay.h"  // for delays
// #include "src/usb_cdc.h" // for USB-CDC serial
// #include "src/lcd1602.h" // for lcd1602
#include "src/tm1637plus.h"

// ===================================================================================
// Main Function
// ===================================================================================

#define true 1
#define false 0

void main(void)
{
  // Setup
  CLK_config(); // configure system clock
  DLY_ms(5);    // wait for clock to stabilize
  tm1637_init();

  while (1)
  {
    // Test brightness
    for (int x = 0; x < 7; x++)
    {
      tm1637_set_brightness(x);
      tm1637_show("8888", false);
      DLY_ms(100);
    }

    // Test segment control
    uint8_t __code seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
    for (uint8_t x = 0; x < 32; ++x)
    {
      uint8_t data = seg_data[x % 6];
      tm1637_set_raw(0, data);
      tm1637_set_raw(1, data);
      tm1637_set_raw(2, data);
      tm1637_set_raw(3, data);
      DLY_ms(100);
    }

    for (uint8_t x = 0; x < 3; ++x)
    {

      // Display time with blinking dots
      for (int z = 0; z < 5; ++z)
      {
        char tmp[5];
        sprintf(tmp, "%.4d", 1225);
        tm1637_set_char(0, tmp[0], z % 2);
        tm1637_set_char(1, tmp[1], z % 2);
        tm1637_set_char(2, tmp[2], z % 2);
        tm1637_set_char(3, tmp[3], z % 2);
        DLY_ms(500);
      }
    }

    // Test display numbers
    for (int x = 0; x < 16; ++x)
    {
      uint8_t show_dot = x % 2; // Show dot every 2nd cycle
      uint8_t v = x < 10 ? x + '0' : x - 10 + 'a';
      tm1637_set_char(0, v, show_dot);
      tm1637_set_char(1, v, show_dot); // On my display "dot" (clock symbol ":") connected only here
      tm1637_set_char(2, v, show_dot);
      tm1637_set_char(3, v, show_dot);
      DLY_ms(100);
    }

    // test scrolling
    // tm1637.scroll("    getting the tm1637 to work in cpp was a lot fun   ");
  }

  /*
    while (1)
    {
      i2clcd_putstr("I2C LCD Tutorial");
      DLY_ms(2000);
      i2clcd_clear();
      i2clcd_putstr("Lets Count 0-10!");
      DLY_ms(2000);
      i2clcd_clear();
      for (int i = 0; i <= 10; ++i)
      {
        char t[2];
        t[0] = '0' + i;
        t[1] = '\0';
        i2clcd_putstr(i == 10 ? "10" : t);
        i2clcd_backlight_on();
        DLY_ms(1000);
        i2clcd_backlight_off();
        DLY_ms(2000);
        i2clcd_clear();
      }
      i2clcd_backlight_on();
    }
    */
}
