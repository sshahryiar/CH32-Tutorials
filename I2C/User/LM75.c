#include "LM75.h"


void LM75_init(void)
{
#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_init();

#elif(I2C_peripheral == use_HW_I2C)
    TWI_init(100000);

#endif

    LM75_write_byte(LM75_pointer_conf_reg, \
                    (LM75_queue_value_1
                    | LM75_OS_POL_LOW \
                    | LM75_OS_Comp \
                    | LM75_normal \
                    | LM75_12_bit_resolution \
                    | LM75_OS_mode_disable));
    Delay_Ms(100);
}


void LM75_write_byte(u8 pointer, u8 value)
{
#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_start();
    SW_I2C_write(LM75_I2C_Address);
    SW_I2C_wait_ACK();
    SW_I2C_write(pointer);
    SW_I2C_wait_ACK();
    SW_I2C_write(value);
    SW_I2C_wait_ACK();
    SW_I2C_stop();

#elif(I2C_peripheral == use_HW_I2C)
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, LM75_I2C_Address, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    I2C_SendData(I2C1, pointer);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_SendData(I2C1, value);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);

#endif
}


s16 LM75_read_byte(u8 pointer)
{
    u8 value = 0;

#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_start();
    SW_I2C_write(LM75_I2C_Address);
    SW_I2C_wait_ACK();
    SW_I2C_write(pointer);
    SW_I2C_wait_ACK();

    SW_I2C_start();
    SW_I2C_write(LM75_I2C_Address | 1);
    SW_I2C_wait_ACK();
    value = SW_I2C_read(I2C_NACK);
    SW_I2C_stop();

#elif(I2C_peripheral == use_HW_I2C)
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, LM75_I2C_Address, I2C_Direction_Transmitter);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, pointer);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, LM75_I2C_Address, I2C_Direction_Receiver);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    while( I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) ==  RESET)
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    value = I2C_ReceiveData(I2C1);
    I2C_GenerateSTOP(I2C1, ENABLE);

#endif

    return value;
}


s16 LM75_read_bytes(u8 pointer)
{
    u8 HB = 0;
    u8 LB = 0;

    s16 value = 0;

#if(I2C_peripheral == use_SW_I2C)
    SW_I2C_start();
    SW_I2C_write(LM75_I2C_Address);
    SW_I2C_wait_ACK();
    SW_I2C_write(pointer);
    SW_I2C_wait_ACK();

    SW_I2C_start();
    SW_I2C_write(LM75_I2C_Address | 1);
    SW_I2C_wait_ACK();
    HB = SW_I2C_read(I2C_ACK);
    LB = SW_I2C_read(I2C_NACK);
    SW_I2C_stop();

#elif(I2C_peripheral == use_HW_I2C)
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) == SET);
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, LM75_I2C_Address, I2C_Direction_Transmitter);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, pointer);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, LM75_I2C_Address, I2C_Direction_Receiver);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    while( I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) ==  RESET)
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    HB = I2C_ReceiveData(I2C1);

    while( I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) ==  RESET)
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    LB = I2C_ReceiveData(I2C1);

    I2C_GenerateSTOP(I2C1, ENABLE);

#endif

    value = ((s16)HB);
    value <<= 8;
    value |= ((s16)LB);

    return value;
}


float LM75_get_temp(void)
{
    float temp = 0;

    temp = (LM75_read_bytes(LM75_pointer_temp_reg) >> 5);
    temp *= 0.125;

    return temp;
}
