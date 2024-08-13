#pragma once


#include "debug.h"


void HW_SPI_GPIO_init(void);
void HW_SPI_init(u8 mode, u16 clk_div, u8 MSB_first);
void HW_SPI_transfer(u8 *tx_bytes, u8 *rx_bytes, u16 packet_size);
