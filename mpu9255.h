//----------------------------------------------------------------
// Invensense MPU9255
//----------------------------------------------------------------

#ifndef MPU9255_H_
#define MPU9255_H_

#include "driver/gpio.h"
#include "rom/ets_sys.h"

class MPU9255
{
public:
    MPU9255( gpio_num_t CS, gpio_num_t SCLK, gpio_num_t MISI, gpio_num_t MISO );

    // this data requires conversions
    struct Raw_Info
    {
        int16_t accel[3];
        int16_t temp;
        int16_t gyro[3];
    };

    bool   raw_read( Raw_Info& raw );
    double raw_temp_to_C( int16_t raw_temp );

private:
    gpio_num_t cs, sclk, misi, miso;

    const uint32_t spi_delay_us = 1000;

    void    spi_write( uint8_t d );
    uint8_t spi_read( void );
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// IMPLEMENTATION
//
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

MPU9255::MPU9255( gpio_num_t CS, gpio_num_t SCLK, gpio_num_t MISI, gpio_num_t MISO ) 
{
    cs   = CS;
    sclk = SCLK;
    misi = MISI;
    miso = MISO;

    gpio_set_direction(sclk, GPIO_MODE_OUTPUT);
    gpio_set_direction(cs,   GPIO_MODE_OUTPUT);
    gpio_set_direction(misi, GPIO_MODE_OUTPUT);
    gpio_set_direction(miso, GPIO_MODE_INPUT);

    gpio_set_level(cs, 1);
}

bool MPU9255::raw_read( Raw_Info& raw )
{
    gpio_set_level( cs, 0 );
    Delay::usec( spi_delay_us );
    spi_write( 0x80 | 0x3b );  // 0x3b is start of raw data

    raw.accel[0] = (spi_read() << 8) | spi_read();
    raw.accel[1] = (spi_read() << 8) | spi_read();
    raw.accel[2] = (spi_read() << 8) | spi_read();
    raw.temp     = (spi_read() << 8) | spi_read();
    raw.gyro[0]  = (spi_read() << 8) | spi_read();
    raw.gyro[1]  = (spi_read() << 8) | spi_read();
    raw.gyro[2]  = (spi_read() << 8) | spi_read();

    gpio_set_level( cs, 1 );
    return true;
}

double MPU9255::raw_temp_to_C( int16_t raw_temp )
{
    return double(raw_temp)/333.87 + 21.0;
}

void MPU9255::spi_write( uint8_t d )
{ 
    for( int i=7; i >= 0; i-- )
    {
        gpio_set_level( sclk, 0 );
        Delay::usec( spi_delay_us );
        gpio_set_level( misi, (d >> i) & 1 );
        gpio_set_level( sclk, 1 );
        Delay::usec( spi_delay_us );
    }
}

uint8_t MPU9255::spi_read( void ) 
{ 
    uint8_t d = 0;

    for( int i=7; i >= 0; i-- )
    {
        gpio_set_level( sclk, 0 );
        Delay::usec( spi_delay_us );
        d |= gpio_get_level( miso ) << i;
        gpio_set_level( sclk, 1 );
        Delay::usec( spi_delay_us );
    }

    return d;
}

#endif // MPU9255
