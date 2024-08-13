#include "PCF8574.h"


u8 PCF8574_read(u8 reg)
{
    u8 value = 0x00;

#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_start();
    SW_I2C_write(PCF8574_I2C_address);
    SW_I2C_wait_ACK();
    SW_I2C_write(reg);
    SW_I2C_wait_ACK();

    SW_I2C_start();
    SW_I2C_write(PCF8574_I2C_address | 1);
    SW_I2C_wait_ACK();
    value = SW_I2C_read(I2C_NACK);
    SW_I2C_stop();

#elif(I2C_peripheral == use_HW_I2C)
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, PCF8574_I2C_address, I2C_Direction_Transmitter);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, PCF8574_I2C_address, I2C_Direction_Receiver);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    while( I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) ==  RESET)
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    value = I2C_ReceiveData(I2C1);
    I2C_GenerateSTOP(I2C1, ENABLE);

#endif

    return value;
}


void PCF8574_write(u8 value)
{
#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_start();
    SW_I2C_write(PCF8574_I2C_address);
    SW_I2C_wait_ACK();
    SW_I2C_write(value);
    SW_I2C_wait_ACK();
    SW_I2C_stop();

#elif(I2C_peripheral == use_HW_I2C)
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, PCF8574_I2C_address, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, value);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);

#endif
}
