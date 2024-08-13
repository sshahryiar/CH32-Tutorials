#pragma once


#include "debug.h"

#define use_SW_I2C      0
#define use_HW_I2C      1

#define I2C_peripheral  use_HW_I2C


#if(I2C_peripheral == use_SW_I2C)
#include "SW_I2C.h"

#elif(I2C_peripheral == use_HW_I2C)
#include "HW_I2C.h"

#endif

#define LM75_I2C_Address                     0x90

//Registers

#define LM75_pointer_temp_reg                0x00
#define LM75_pointer_conf_reg                0x01
#define LM75_pointer_thyst_reg               0x02
#define LM75_pointer_tos_reg                 0x03

//Configure Register Bits

#define LM75_queue_value_1                   0x00
#define LM75_queue_value_2                   0x08
#define LM75_queue_value_4                   0x10
#define LM75_queue_value_6                   0x18

#define LM75_9_bit_resolution                0x00
#define LM75_10_bit_resolution               0x20
#define LM75_11_bit_resolution               0x40
#define LM75_12_bit_resolution               0x60

#define LM75_OS_POL_LOW                      0x00
#define LM75_OS_POL_HIGH                     0x04

#define LM75_OS_Comp                         0x00
#define LM75_OS_INT                          0x02

#define LM75_normal                          0x00
#define LM75_shutdown                        0x01

#define LM75_OS_mode_disable                 0x00
#define LM75_OS_mode_enable                  0x80


void LM75_init(void);
void LM75_write_byte(u8 pointer, u8 value);
s16 LM75_read_byte(u8 pointer);
s16 LM75_read_bytes(u8 pointer);
float LM75_get_temp(void);
