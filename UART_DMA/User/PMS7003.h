#pragma once


#include "debug.h"
#include "string.h"


#define PMS7003_Rx_Buffer           32

#define PMS7003_start_byte_1        0x42
#define PMS7003_start_byte_2        0x4D

#define PMS7003_change_mode_cmd     0xE1
#define PMS7003_passive_read_cmd    0xE2
#define PMS7003_sleep_wakeup_cmd    0xE4

#define PMS7003_passive_mode        0x00
#define PMS7003_active_mode         0x01

#define PMS7003_sleep_mode          0x00
#define PMS7003_wakeup_mode         0x01

#define PMS7003_okay                0
#define PMS7003_CRC_error           1
#define PMS7003_frame_error         2
#define PMS7003_data_missing        3


void USART_init(void);
void DMA_init(void);
u16 make_word(u8 HB, u8 LB);
void PMS7003_init(void);
void PMS7003_send_command(u8 cmd, u8 dat);
void PMS7003_passive_mode_read(void);
void PMS7003_active_mode_read(void);
void PMS7003_sleep(void);
void PMS7003_wakeup(void);
u8 PMS7003_read_raw(void);
u8 PMS7003_read_sensor(u8 sector, u16 *data_1, u16 *data_2, u16 *data_3);
