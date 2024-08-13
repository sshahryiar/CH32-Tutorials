#include "debug.h"



#define DS18B20_convert_T                       0x44
#define DS18B20_read_scratchpad                 0xBE
#define DS18B20_write_scratchpad                0x4E
#define DS18B20_copy_scratchpad                 0x48
#define DS18B20_recall_E2                       0xB8
#define DS18B20_read_power_supply               0xB4
#define DS18B20_skip_ROM                        0xCC

#define DS18B20_resolution                      12


void DS18B20_init(void);
s32 DS18B20_get_temperature(void);
