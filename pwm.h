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

    timer_conf.bit_num    = LEDC_TIMER_15_BIT;          // always for now
    timer_conf.freq_hz    = hz;
    timer_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    timer_conf.timer_num  = LEDC_TIMER_0 + index;
    ledc_timer_config( &timer_conf );

    ledc_conf.channel     = LEDC_CHANNEL_0 + index;
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
