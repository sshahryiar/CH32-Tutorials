#include "OW.h"
#include "DS18B20.h"


void DS18B20_init(void)
{
    onewire_reset();
    Delay_Ms(100);
}


s32 DS18B20_get_temperature(void)
{
    u8 msb = 0x00;
    u8 lsb = 0x00;
    double temp = 0;

    onewire_reset();
    onewire_write(DS18B20_skip_ROM);
    onewire_write(DS18B20_convert_T);

    switch(DS18B20_resolution)
    {
        case 12:
        {
            Delay_Ms(750);
            break;
        }
        case 11:
        {
            Delay_Ms(375);
            break;
        }
        case 10:
        {
            Delay_Ms(188);
            break;
        }
        default:
        {
            Delay_Ms(94);
            break;
        }
    }

    onewire_reset();

    onewire_write(DS18B20_skip_ROM);
    onewire_write(DS18B20_read_scratchpad);

    lsb = onewire_read();
    msb = onewire_read();

    temp = (double)msb;
    temp *= 256.0;
    temp += (double)lsb;

    temp *= 10.0;

    switch(DS18B20_resolution)
    {
        case 12:
        {
            temp *= 0.0625;
            break;
        }
        case 11:
        {
            temp *= 0.125;
            break;
        }
        case 10:
        {
            temp *= 0.25;
            break;
        }
        default:
        {
            temp *= 0.5;
            break;
        }
    }

    Delay_Ms(40);

    return ((s32)temp);
}
