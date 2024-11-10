// ===================================================================================
// Project:   I2C LCD1602/2004 for CH551, CH552, CH554
// Version:   v1.1
// Year:      2024
// Author:    Francois Blanchette
// Github:    https://github.com/cfrankb
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// This code implements an interface for LCD1602/2004 displays based on the
// original micropython version.
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
//    LCD1602 / 2004
//
//    SDA    <-->      P16 (MISO)
//    SCL    <-->      P17 (SCK)
//    GND    <-->      GND
//    VCC    <-->      VCC *
//
//    * Most LCD1602/2004 operate on 5v. Some variants might use only 3v.
//      Please check documentation for details.
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
// - To compile the firmware using the Arduino IDE, follow the instructions in the
//   .ino file.
//

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"  // user configurations
#include "src/system.h"  // system functions
#include "src/gpio.h"    // for GPIO
#include "src/delay.h"   // for delays
#include "src/lcd1602.h" // for lcd1602

// ===================================================================================
// Main Function
// ===================================================================================

void main(void)
{
  // Setup
  CLK_config(); // configure system clock
  DLY_ms(5);    // wait for clock to stabilize

  i2clcd_init(2, 16);
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
}
