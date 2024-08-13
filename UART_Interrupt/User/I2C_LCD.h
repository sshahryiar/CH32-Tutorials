#pragma once

#include "debug.h"
#include "PCF8574.h"
#include "stdio.h"


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

#define BL_ON                    1
#define BL_OFF                   0

#define DAT                      1
#define CMD                      0


void LCD_init(void);
void LCD_toggle_EN(void);
void LCD_send(u8 value, u8 mode);
void LCD_4bit_send(u8 lcd_data);
void LCD_putchar(char char_data);
void LCD_clear_home(void);
void LCD_goto(u8 x_pos, u8 y_pos);
void LCD_putstr(u8 x_pos, u8 y_pos, char *lcd_string);
