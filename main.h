// Spin Coater
//
// This example code is in the Public Domain.
//
// Unless required by applicable law or agreed to in writing, this
// software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied.
//-------------------------------------------------------------------

#include <iostream>
#include <cstdio>
#include <string>

#include "delay.h"              
#include "math.h"
#include "pwm.h"

// Motor/ESC Parameters
const double motor_kv           = 2600.0;
const double motor_v            = 11.1;   // voltage
const int    motor_pole_cnt     = 12;   
const double motor_rpm_min      = 0;
const double motor_rpm_max      = motor_kv * motor_v;

const gpio_num_t esc_pin        = GPIO_NUM_18;
const double esc_rpm_pole_1     = 480000;
const double esc_rpm_max        = esc_rpm_pole_1 / double(motor_pole_cnt);  // should be at least motor_rpm_max
const double esc_cycle_ms       = 20.0;
const double esc_hz             = 1000.0 / esc_cycle_ms;

const double esc_duty_pct_min   = 1.0 / esc_cycle_ms * 100.0;  // for motor_rpm_min (I think)
const double esc_duty_pct_max   = 2.0 / esc_cycle_ms * 100.0;  // for motor_rpm_max (I think)

// Profiles for Various Solder Pastes
//
struct Point 
{
    int   time_sec;             // when?
    int   RPM_target;           // target RPM at this time
};

struct Profile 
{
    const char *        name;
    const Point *       points;
    int                 point_cnt;     
};

const Point pcb_photoresist[] = 
{
    {  30,  100 },  // slow RPM to allow for dispensing liquid
    {  90, 1000 },
    { 180, 5000 },
    { 270, 5000 },
    { 360, 1000 },
    { 450,    0 },
    { 0x7fffffff, 0 }, // stay off for 68 years :-)
};

const Profile profiles[] = 
{
    { "PCB Photo Resist",
      pcb_photoresist,
      sizeof( pcb_photoresist ) / sizeof( pcb_photoresist[0] ) },
};

const int profile_cnt = sizeof( profiles ) / sizeof( profiles[0] );

int main()
{
    // ESC PWM Controller
    PWM esc_pwm( esc_pin, esc_hz );

    // Choose Profile
    const Profile& profile = profiles[0];  // PCB photoresist

    int   RPM_start = 0;
    int   RPM = RPM_start;
    int   t = 0;       // in seconds
    int   t_start = 0; 
    int   p = -1;      // point index
    for( ;; ) 
    {
        // tick one second
        Delay::sec( 1 );
        t++;

        // see if we need to move to the next point in the profile
        if ( p == -1 || t >= profile.points[p].time_sec ) {
            t_start = t - 1;
            p++;
            if ( p == profile.point_cnt ) break;
            if ( p > 0 ) RPM_start = profile.points[p-1].RPM_target;
        }

        // Figure out the new slope to use to hit the target RPM.
        // Then figure out the target at this point in time.
        double slope = double( profile.points[p].RPM_target - RPM_start ) / double( profile.points[p].time_sec - t_start );
        RPM = double( t - t_start )*slope + RPM_start;

        std::cout << "\n";
        std::cout << "Profile:  " << profile.name << "\n";
        std::cout << "Time:     " << t << " secs\n";
        std::cout << "RPM:      " << RPM << "\n";

        // change the RPM
        double rpm_frac = (RPM - motor_rpm_min) / (motor_rpm_max - motor_rpm_min);
        double duty_pct = esc_duty_pct_min + rpm_frac*(esc_duty_pct_max - esc_duty_pct_min);
        esc_pwm.setDutyPct( duty_pct );
    }

    std::cout << "DONE\n";
    return 0;
}
