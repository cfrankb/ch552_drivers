#include "lcd1602.h"
#include "i2c.h"
#include "delay.h"

#define _delay DLY_ms
#define _hal_sleep_us DLY_us
#define true 1
#define false 0
#define I2C_ADDR (0x27 << 1)

// # PCF8574 pin definitions
enum
{
    MASK_RS = 0x01,             // # P0
    MASK_RW = 0x02,             // # P1
    MASK_E = 0x04,              // # P2
    SHIFT_BACKLIGHT = 3,        // # P3
    SHIFT_DATA = 4,             // # P4-P7
                                //
    LCD_CLR = 0x01,             //   DB0: clear display
    LCD_HOME = 0x02,            //   DB1: return to home position
    LCD_ENTRY_MODE = 0x04,      //   DB2: set entry mode
    LCD_ENTRY_INC = 0x02,       //   --DB1: increment
    LCD_ENTRY_SHIFT = 0x01,     //   --DB0: shift
                                //
    LCD_ON_CTRL = 0x08,         //    DB3: turn lcd/cursor on
    LCD_ON_DISPLAY = 0x04,      //    --DB2: turn display on
    LCD_ON_CURSOR = 0x02,       //    --DB1: turn cursor on
    LCD_ON_BLINK = 0x01,        //    --DB0: blinking cursor
    LCD_MOVE = 0x10,            //    DB4: move cursor/display
    LCD_MOVE_DISP = 0x08,       //    --DB3: move display (0-> move cursor)
    LCD_MOVE_RIGHT = 0x04,      //    --DB2: move right (0-> left)
                                //
    LCD_FUNCTION = 0x20,        //  DB5: function set
    LCD_FUNCTION_8BIT = 0x10,   //  --DB4: set 8BIT mode (0->4BIT mode)
    LCD_FUNCTION_2LINES = 0x08, //  --DB3: two lines (0->one line)
    LCD_FUNCTION_10DOTS = 0x04, //  --DB2: 5x10 font (0->5x7 font)
    LCD_FUNCTION_RESET = 0x30,  //  See "Initializing by Instruction" section
                                //
    LCD_CGRAM = 0x40,           //  DB6: set CG RAM address
    LCD_DDRAM = 0x80,           //  DB7: set DD RAM address
    LCD_RS_CMD = 0,             //
    LCD_RS_DATA = 1,            //
                                //
    LCD_RW_WRITE = 0,           //
    LCD_RW_READ = 1,            //
};

static __xdata uint8_t m_backlight = 1;
static __xdata uint8_t m_cursor_x = 0;
static __xdata uint8_t m_cursor_y = 0;
static __xdata uint8_t m_num_lines = 0;
static __xdata uint8_t m_num_columns = 0;
static __xdata uint8_t m_implied_newline = 0;

static void _hal_write_init_nibble(const uint8_t nibble);
static void _hal_write_command(const uint8_t cmd);

static void _write(const uint8_t v)
{
    I2C_start(I2C_ADDR);
    I2C_write(v);
    I2C_stop();
}

/// @brief  Writes an initialization nibble to the LCD.
/// @param nibble
static void _hal_write_init_nibble(const uint8_t nibble)
{
    //  This particular function is only used during initialization.
    const uint8_t byte = ((nibble >> 4) & 0x0f) << SHIFT_DATA;
    _write(byte | MASK_E);
    _write(byte);
}

/// @brief Write a command to the LCD. Data is latched on the falling edge of E.
/// @param cmd
static void _hal_write_command(const uint8_t cmd)
{

    uint8_t byte = ((m_backlight << SHIFT_BACKLIGHT) |
                    (((cmd >> 4) & 0x0f) << SHIFT_DATA));
    _write(byte | MASK_E);
    _write(byte);

    byte = ((m_backlight << SHIFT_BACKLIGHT) |
            ((cmd & 0x0f) << SHIFT_DATA));
    _write(byte | MASK_E);
    _write(byte);

    if (cmd <= 3)
    {
        // The home and clear commands require a worst case delay of 4.1 msec
        _delay(5);
    }
}

/// @brief Write data to the LCD. Data is latched on the falling edge of E.
/// @param data
static void _hal_write_data(const uint8_t data)
{
    uint8_t byte = (MASK_RS |
                    (m_backlight << SHIFT_BACKLIGHT) |
                    (((data >> 4) & 0x0f) << SHIFT_DATA));
    _write(byte | MASK_E);
    _write(byte);

    byte = (MASK_RS |
            (m_backlight << SHIFT_BACKLIGHT) |
            ((data & 0x0f) << SHIFT_DATA));
    _write(byte | MASK_E);
    _write(byte);
}

/// @brief Allows the hal layer to turn the backlight on
static void _hal_backlight_on(void)
{
    _write(1 << SHIFT_BACKLIGHT);
}

/// @brief Allows the hal layer to turn the backlight off
static void _hal_backlight_off(void)
{
    _write(0);
}

/// @brief Clears the LCD display and moves the cursor to the top left corner.
void i2clcd_clear(void)
{
    _hal_write_command(LCD_CLR);
    _hal_write_command(LCD_HOME);
    m_cursor_x = 0;
    m_cursor_y = 0;
}

/// @brief Causes the cursor to be hidden.
void i2clcd_hide_cursor(void)
{
    _hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY);
}

/// @brief Turns on the cursor, and makes it blink.
void i2clcd_blink_cursor_on(void)
{
    _hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY |
                       LCD_ON_CURSOR | LCD_ON_BLINK);
}

/// @brief Turns on the cursor, and makes it no blink (i.e. be solid).
void i2clcd_blink_cursor_off(void)
{
    _hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY |
                       LCD_ON_CURSOR);
}

/// @brief Turns on (i.e. unblanks) the LCD.
void i2clcd_display_on(void)
{
    _hal_write_command(LCD_ON_CTRL | LCD_ON_DISPLAY);
}

/// @brief Turns off (i.e. blanks) the LCD.
void i2clcd_display_off(void)
{
    _hal_write_command(LCD_ON_CTRL);
}

/// @brief Turns the backlight on.
void i2clcd_backlight_on(void)
{
    // This isn't really an LCD command, but some modules have backlight
    // controls, so this allows the hal to pass through the command.
    m_backlight = true;
    _hal_backlight_on();
}

/// @brief Turns the backlight off.
void i2clcd_backlight_off(void)
{
    //  This isn't really an LCD command, but some modules have backlight
    //  controls, so this allows the hal to pass through the command.
    m_backlight = false;
    _hal_backlight_off();
}

/// @brief Moves the cursor position to the indicated position. The cursor
///        position is zero based (i.e. cursor_x == 0 indicates first column).
/// @param cursor_x
/// @param cursor_y
void i2clcd_move_to(const uint8_t cursor_x, const uint8_t cursor_y)
{
    m_cursor_x = cursor_x;
    m_cursor_y = cursor_y;
    uint16_t addr = cursor_x & 0x3f;
    if (cursor_y & 1)
    {
        addr += 0x40; // # Lines 1 & 3 add 0x40
    }
    if (cursor_y & 2)
    { //    # Lines 2 & 3 add number of columns
        addr += m_num_columns;
    }
    _hal_write_command(LCD_DDRAM | addr);
}

/// @brief Writes the indicated character to the LCD at the current cursor
///        position, and advances the cursor by one position.
/// @param ch ascii character
void i2clcd_putchar(const char ch)
{
    if (ch == '\n')
    {
        if (!m_implied_newline)
        {
            // implied_newline means we advanced due to a wraparound,
            // so if we get a newline right after that we ignore it.
            m_cursor_x = m_num_columns;
        }
    }
    else
    {
        _hal_write_data(ch);
        m_cursor_x += 1;
    }

    if (m_cursor_x >= m_num_columns)
    {
        m_cursor_x = 0;
        m_cursor_y += 1;
        m_implied_newline = (ch != '\n');
    }

    if (m_cursor_y >= m_num_lines)
        m_cursor_y = 0;
    i2clcd_move_to(m_cursor_x, m_cursor_y);
}

/// @brief Write the indicated string to the LCD at the current cursor
///        position and advances the cursor position appropriately.
/// @param s string to be written
void i2clcd_putstr(const char *s)
{
    for (int i = 0; s[i]; ++i)
    {
        i2clcd_putchar(s[i]);
    }
}

/// @brief Write a character to one of the 8 CGRAM locations, available
///        as chr(0) through chr(7).
/// @param location
/// @param charmap
void i2clcd_custom_char(const int location, const uint8_t charmap[8])
{
    _hal_write_command(LCD_CGRAM | ((location & 0x7) << 3));
    _hal_sleep_us(40);
    for (int i = 0; i < 8; ++i)
    {
        _hal_write_data(charmap[i]);
        _hal_sleep_us(40);
    }
    i2clcd_move_to(m_cursor_x, m_cursor_y);
}

/// @brief late stage initialization
/// @param
static void _postInit(void)
{
    i2clcd_display_off();
    i2clcd_backlight_on();
    i2clcd_clear();
    _hal_write_command(LCD_ENTRY_MODE | LCD_ENTRY_INC);
    i2clcd_hide_cursor();
    i2clcd_display_on();
}

/// @brief initialize the i2c interface for lcd1602/2004
/// @param num_lines
/// @param num_columns
void i2clcd_init(const uint8_t num_lines, const uint8_t num_columns)
{
    m_num_lines = num_lines > 4 ? 4 : num_lines;
    m_num_columns = num_columns > 40 ? 40 : num_columns;

    I2C_init(); // initialize I2C first

    I2C_write(0);
    _delay(20); // Allow LCD time to powerup

    // Send reset 3 times
    _hal_write_init_nibble(LCD_FUNCTION_RESET);
    _delay(5); // Need to delay at least 4.1 msec
    _hal_write_init_nibble(LCD_FUNCTION_RESET);
    _delay(1);
    _hal_write_init_nibble(LCD_FUNCTION_RESET);
    _delay(1);
    // Put LCD into 4-bit mode
    _hal_write_init_nibble(LCD_FUNCTION);
    _delay(1);
    _postInit();
    const uint8_t cmd = m_num_lines > 1 ? LCD_FUNCTION | LCD_FUNCTION_2LINES : LCD_FUNCTION;
    _hal_write_command(cmd);
}
