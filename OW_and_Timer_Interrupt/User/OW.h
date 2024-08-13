#include "debug.h"


#define OW_IN                     GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)

#define OW_OUT_LOW                GPIO_WriteBit(GPIOD, GPIO_Pin_3, RESET)
#define OW_OUT_HIGH               GPIO_WriteBit(GPIOD, GPIO_Pin_3, SET)

#define TRUE                       1
#define FALSE                      0


void OW_IN_init(void);
void OW_OUT_init(void);
u8 onewire_reset(void);
void onewire_write_bit(u8 bit_value);
u8 onewire_read_bit(void);
void onewire_write(u8 value);
u8 onewire_read(void);
