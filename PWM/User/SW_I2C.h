#pragma once

#include "debug.h"


#define SDA_HIGH()      GPIO_SetBits(GPIOC, GPIO_Pin_1)
#define SDA_LOW()       GPIO_ResetBits(GPIOC, GPIO_Pin_1)

#define SCL_HIGH()      GPIO_SetBits(GPIOC, GPIO_Pin_2)
#define SCL_LOW()       GPIO_ResetBits(GPIOC, GPIO_Pin_2)

#define SDA_IN()        GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)

#define I2C_ACK         0xFF
#define I2C_NACK        0x00

#define I2C_timeout     60000


void SW_I2C_GPIO_init(void);
void SW_I2C_SDA_OUT_init(void);
void SW_I2C_SDA_IN_init(void);
void SW_I2C_init(void);
void SW_I2C_start(void);
void SW_I2C_stop(void);
u8 SW_I2C_read(u8 ack);
void SW_I2C_write(u8 value);
void SW_I2C_ACK_NACK(u8 mode);
u8 SW_I2C_wait_ACK(void);
