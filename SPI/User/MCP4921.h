#pragma once


#include "debug.h"


#define use_SW_SPI      0
#define use_HW_SPI      1

#define SPI_peripheral  use_HW_SPI


#if(SPI_peripheral == use_SW_SPI)
#include "SW_SPI.h"

#elif(SPI_peripheral == use_HW_SPI)
#include "HW_SPI.h"

#endif

#define MCP4921_DAC_A_CMD       0x00
#define MCP4921_DAC_B_CMD       0x80

#define MCP4921_BUF_CMD         0x40
#define MCP4921_UNBUF_CMD       0x00

#define MCP4921_1x_GAIN_CMD     0x20
#define MCP4921_2x_GAIN_CMD     0x00

#define MCP4921_PDC_CMD         0x10
#define MCP4921_HIZ_CMD         0x00

#define MCP4921_CMD             (MCP4921_DAC_A_CMD | MCP4921_BUF_CMD | MCP4921_1x_GAIN_CMD | MCP4921_PDC_CMD)


void MCP4921_init(void);
void MCP4921_write(u8 cmd, u16 dac);
void MCP4921_write_DAC(u16 value);
