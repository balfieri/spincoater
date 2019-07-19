//---------------------------------------------------
// MAX6675 Thermocouple Sensor
//
// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple
//---------------------------------------------------

#ifndef MAX6675_H_
#define MAX6675_H_

#include <cmath>

#include "delay.h"
#include "driver/gpio.h"

class MAX6675 {
public:
    MAX6675(gpio_num_t CS, gpio_num_t SCLK, gpio_num_t MISO);

    double readC(void); // Celsius
    double readF(void) { return readC() * 9.0/5.0 + 32; }

private:
    gpio_num_t cs, sclk, miso;
    uint8_t spiread(void);
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

MAX6675::MAX6675(gpio_num_t CS, gpio_num_t SCLK, gpio_num_t MISO) {
    cs = CS;
    sclk = SCLK;
    miso = MISO;

    gpio_set_direction(sclk, GPIO_MODE_OUTPUT);
    gpio_set_direction(cs,   GPIO_MODE_OUTPUT);
    gpio_set_direction(miso, GPIO_MODE_INPUT);

    gpio_set_level(cs, 1);
}

double MAX6675::readC(void) {
    uint16_t v;

    gpio_set_level(cs, 0);
    Delay::msec(1);

    v = spiread();
    v <<= 8;
    v |= spiread();

    gpio_set_level(cs, 1);

    if (v & 0x4) {
        // no thermocouple attached
        return NAN; 
    }

    v >>= 3;

    return v*0.25;
}

uint8_t MAX6675::spiread(void) { 
    int i;
    uint8_t d = 0;

    for (i=7; i>=0; i--)
    {
        gpio_set_level(sclk, 0);
        Delay::msec(1);
        if (gpio_get_level(miso)) {
            d |= (1 << i);
        }

        gpio_set_level(sclk, 1);
        Delay::msec(1);
    }

    return d;
}

#endif // MAX6675_H_
