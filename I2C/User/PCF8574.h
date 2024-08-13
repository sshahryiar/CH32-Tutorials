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


#define PCF8574_I2C_address     0x4E


u8 PCF8574_read(u8 reg);
void PCF8574_write(u8 value);
