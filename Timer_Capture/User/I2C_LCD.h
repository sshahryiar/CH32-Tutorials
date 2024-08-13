#include "debug.h"
#include "MCP23017.h"
#include "stdio.h"


/*

Pin out of MCP23017 to LCD

DIYMORE MCP23017 1602/2004/12864 I2C LCD Driver Module Pin Mapping
------------------------------------------------------------------

GPIOA
    A7   A6   A5   A4   A3   A2   A1   A0
    RS   RW   EN   X    X    X    GND  X

GPIOB
    B7   B6   B5   B4   B3   B2   B1   B0
    D7   D6   D5   D4   D3   D2   D1   D0


ROBOTDYN MCP23017 1602/2004/12864 I2C LCD Driver Module Pin Mapping
-------------------------------------------------------------------

GPIOA
    A7   A6   A5   A4   A3   A2   A1   A0
    EN   RW   RS   X    L-   L+   X    X

GPIOB
    B7   B6   B5   B4   B3   B2   B1   B0
    D7   D6   D5   D4   D3   D2   D1   D0

*/


#define clear_display            0x01
#define goto_home                0x02

#define cursor_direction_inc     0x06
#define cursor_direction_dec     0x04
#define display_shift            0x05
#define display_no_shift         0x04

#define display_on               0x0C
#define display_off              0x0A
#define cursor_on                0x0A
#define cursor_off               0x08
#define blink_on                 0x09
#define blink_off                0x08

#define _8_pin_interface         0x30
#define _4_pin_interface         0x20
#define _2_row_display           0x28
#define _1_row_display           0x20
#define _5x10_dots               0x60
#define _5x7_dots                0x20

#define line_1_y_pos             0x00
#define line_2_y_pos             0x40
#define line_3_y_pos             0x14
#define line_4_y_pos             0x54

#define DAT                      1
#define CMD                      0

#define DIYMORE                  1
#define ROBOTDYN                 0

#define LCD_8_bit_mode           1
#define LCD_4_bit_mode           0

#define LCD_BIT_MODE             LCD_8_bit_mode
#define MODEL                    DIYMORE


void LCD_init(void);
void toggle_EN(void);
void LCD_send(u8 value, u8 type);
void LCD_write_data(u8 value);
void LCD_Clear_Home(void);
void LCD_goto_xy(u8 x_pos, u8 y_pos);
void LCD_putchr(char ch);
void LCD_putstr(u8 x_pos, u8 y_pos, char *lcd_string);
