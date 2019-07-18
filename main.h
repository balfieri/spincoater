// Internet of Things App
//
// This example code is in the Public Domain.
//
// Unless required by applicable law or agreed to in writing, this
// software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied.
//-------------------------------------------------------------------

#include <iostream>
#include <string>

#include "math.h"
#include "oled.h"
#include "max6675.h"            // temperature sensor

int main()
{
    //             RST          SCL          SDA       Resolution    I2C Addr
    OLED oled( GPIO_NUM_16, GPIO_NUM_15, GPIO_NUM_4, SSD1306_128x64, 0x3c );
    if ( oled.init() ) {
        oled.clear();
        oled.select_font( 1 );
        oled.draw_string( 0, 0, "Hi, OLED!", WHITE, BLACK );
        oled.refresh( true );
    } 

    // sensors
    //                      SCLK         CS           MISO
    MAX6675 temp( GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_14 );

    for( ;; )
    {
        double C = temp.readC();
        std::cout << "Temperature: ";
        if ( __isnand(C) ) {
            std::cout << "no thermocouple attached!\n";
        } else {
            std::cout << C << "C\n";
        }
        ets_delay_us( 1000000 );
    }

    return 0;
}
