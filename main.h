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
    // SERVO PWM
    int relay_n = 1;   // off
    gpio_num_t relay_pin = GPIO_NUM_25;
    gpio_set_direction( relay_pin, GPIO_MODE_OUTPUT );
    gpio_set_level( relay_pin, relay_n );

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
        float slope   = float( profile.points[p].RPM_target - RPM_start ) / float( profile.points[p].time_sec - t_start );
        RPM = float( t - t_start )*slope + RPM_start;

        std::cout << "\n";
        std::cout << "Profile:  " << profile.name << "\n";
        std::cout << "Time:     " << t << " secs\n";
        std::cout << "RPM:      " << RPM << "\n";

        // change the RPM
    }

    std::cout << "DONE\n";
    return 0;
}
