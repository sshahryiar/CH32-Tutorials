#include "I2C_LCD.h"


static u8 lcd_ctrl;


void LCD_init(void)
{
    lcd_ctrl = 0x00;

#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_init();

#elif(I2C_peripheral == use_HW_I2C)
    TWI_init(100000);

#endif
    MCP23017_init();

#if(LCD_BIT_MODE == LCD_8_bit_mode)
    LCD_send((_8_pin_interface | _2_row_display | _5x7_dots), CMD);
    Delay_Ms(10);
    LCD_send((_8_pin_interface | _2_row_display | _5x7_dots), CMD);
    Delay_Ms(10);
    LCD_send((_8_pin_interface | _2_row_display | _5x7_dots), CMD);
#elif(LCD_BIT_MODE == LCD_4_bit_mode)
    LCD_send((_4_pin_interface | _2_row_display | _5x7_dots), CMD);
    Delay_Ms(10);
    LCD_send((_4_pin_interface | _2_row_display | _5x7_dots), CMD);
    Delay_Ms(10);
    LCD_send((_4_pin_interface | _2_row_display | _5x7_dots), CMD);
#endif

    LCD_send((display_on | cursor_off | blink_off), CMD);
    LCD_send(clear_display, CMD);
    LCD_send((cursor_direction_inc | display_no_shift), CMD);
}


void toggle_EN(void)
{
#if(MODEL == DIYMORE)
    lcd_ctrl |= 0x22;
#elif(MODEL == ROBOTDYN)
    lcd_ctrl |= 0x80;
#endif
    MCP23017_write(MCP23017_GPIOA, lcd_ctrl);
    Delay_Ms(1);
#if(MODEL == DIYMORE)
    lcd_ctrl &= 0xDF;
#elif(MODEL == ROBOTDYN)
    lcd_ctrl &= 0x7F;
#endif
    MCP23017_write(MCP23017_GPIOA, lcd_ctrl);
    Delay_Ms(1);
}


void LCD_send(u8 value, u8 type)
{
    switch(type)
    {
        case DAT:
        {
#if(MODEL == DIYMORE)
            lcd_ctrl |= 0x82;
#elif(MODEL == ROBOTDYN)
            lcd_ctrl |= 0x20;
#endif
            break;
        }
        default:
        {
#if(MODEL == DIYMORE)
            lcd_ctrl &= 0x7F;
#elif(MODEL == ROBOTDYN)
            lcd_ctrl &= 0xDF;
#endif
            break;
        }
    }

    MCP23017_write(MCP23017_GPIOA, lcd_ctrl);
    LCD_write_data(value);
}


void LCD_write_data(u8 value)
{
#if(LCD_BIT_MODE == LCD_8_bit_mode)
    MCP23017_write(MCP23017_GPIOB, value);
    toggle_EN();
#elif(LCD_BIT_MODE == LCD_4_bit_mode)
    MCP23017_write(MCP23017_GPIOB, (value & 0xF0));
    toggle_EN();
    MCP23017_write(MCP23017_GPIOB, ((value & 0x0F) << 0x04));
    toggle_EN();
#endif
}


void LCD_Clear_Home(void)
{
    LCD_send(clear_display, CMD);
    LCD_send(goto_home, CMD);
}


void LCD_goto_xy(u8 x_pos, u8 y_pos)
{
    switch(y_pos)
    {
        case 1:
        {
            LCD_send((0x80 | (line_2_y_pos + x_pos)), CMD);
            break;
        }

        case 2:
        {
            LCD_send((0x80 | (line_3_y_pos + x_pos)), CMD);
            break;
        }

        case 3:
        {
            LCD_send((0x80 | (line_4_y_pos + x_pos)), CMD);
            break;
        }

        default:
        {
            LCD_send((0x80 | (line_1_y_pos + x_pos)), CMD);
            break;
        }
    }
}


void LCD_putchr(char ch)
{
    LCD_send(ch, DAT);
}


void LCD_putstr(u8 x_pos, u8 y_pos, char *lcd_string)
{
    LCD_goto_xy(x_pos, y_pos);
    while (*lcd_string != '\0')
    {
        LCD_putchr(*lcd_string++);
    };
}
