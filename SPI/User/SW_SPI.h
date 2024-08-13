#pragma once


#include "debug.h"


#define spi_mode_0      0
#define spi_mode_1      1
#define spi_mode_2      2
#define spi_mode_3      3

#define sw_spi_mode     spi_mode_0

void SW_SPI_GPIO_init(void);
u8 SW_SPI_send_receive(u8 value);
void SW_SPI_transfer(u8 *tx_bytes, u8 *rx_bytes, u16 packet_size);
