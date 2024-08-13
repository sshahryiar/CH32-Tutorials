#include "debug.h"


#define HMC1022_Default_I2C_Address                                                0xE0

#define HMC1022_Get_Angular_Measurement                                            0x31
#define HMC1022_Start_Calibration                                                  0xC0
#define HMC1022_End_Calibration                                                    0xC1
#define HMC1022_Set_Magnetic_Declination_High_Byte                                 0x03
#define HMC1022_Set_Magnetic_Declination_Low_Byte                                  0x04

#define HMC1022_no_of_data_bytes_returned                                          0x08

#define HMC1022_timeout                                                            1000


void cal_LED_init(void);
void UART_write(u8 value);
u16 HMC1022_read_heading(void);
void HMC1022_calibrate_compass(void);
void HMC1022_factory_reset();
void HMC1022_set_I2C_address(u8 i2c_address);
void HMC1022_set_declination_angle(u16 angle);
