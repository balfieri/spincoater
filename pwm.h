//---------------------------------------------------
// PWM Controller for Servos and LEDs
//
// This library is public domain.
//---------------------------------------------------

#ifndef PWM_H_
#define PWM_H_

#include <cmath>

#include "delay.h"
#include "driver/gpio.h"
#include "driver/ledc.h"                // PWM for LEDs can be used for other things like motors

class PWM 
{
public:
    PWM( gpio_num_t PIN, double HZ, int INDEX=0 );

    void setDutyPct( double pct );      // percentage of 1/hz cycle

private:
    gpio_num_t            pin;
    double                hz;           // cycle/timer frequency
    int                   index;
    ledc_timer_config_t   timer_conf;
    ledc_channel_config_t ledc_conf;
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

PWM::PWM( gpio_num_t PIN, double HZ, int INDEX )
{
    pin      = PIN;
    hz       = HZ;
    index    = INDEX;

    timer_conf.duty_resolution = LEDC_TIMER_15_BIT;          // always for now
    timer_conf.freq_hz    = hz;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    switch( index ) 
    {
        case 0: timer_conf.timer_num = LEDC_TIMER_0; break;
        case 1: timer_conf.timer_num = LEDC_TIMER_1; break;
        case 2: timer_conf.timer_num = LEDC_TIMER_2; break;
        case 3: timer_conf.timer_num = LEDC_TIMER_3; break;
        default: std::cout << "ERROR: bad index for timer_num, using 0\n"; timer_conf.timer_num = LEDC_TIMER_0; break;
    }
    ledc_timer_config( &timer_conf );

    switch( index ) 
    {
        case 0: ledc_conf.channel = LEDC_CHANNEL_0; break;
        case 1: ledc_conf.channel = LEDC_CHANNEL_1; break;
        case 2: ledc_conf.channel = LEDC_CHANNEL_2; break;
        case 3: ledc_conf.channel = LEDC_CHANNEL_3; break;
        case 4: ledc_conf.channel = LEDC_CHANNEL_4; break;
        case 5: ledc_conf.channel = LEDC_CHANNEL_5; break;
        case 6: ledc_conf.channel = LEDC_CHANNEL_6; break;
        case 7: ledc_conf.channel = LEDC_CHANNEL_7; break;
        default: std::cout << "ERROR: bad index for ledc_num, using 0\n"; ledc_conf.channel = LEDC_CHANNEL_0; break;
    }
    ledc_conf.gpio_num    = pin;
    ledc_conf.intr_type   = LEDC_INTR_DISABLE;
    ledc_conf.speed_mode  = LEDC_HIGH_SPEED_MODE;
    ledc_conf.timer_sel   = timer_conf.timer_num;
    setDutyPct( 0.0 );
}

void PWM::setDutyPct( double pct )
{
    ledc_conf.duty = pct * double( (1 << 15)-1 ) / 100.0;
    ledc_channel_config( &ledc_conf );
}

#endif // PWM_H_
