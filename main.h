// Internet of Things App
//
// This example code is in the Public Domain.
//
// Unless required by applicable law or agreed to in writing, this
// software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied.
//-------------------------------------------------------------------

#include <iostream>

#include "oled.h"

int main()
{
    //             RST          SCL          SDA       Resolution    I2C Addr
    OLED oled( GPIO_NUM_16, GPIO_NUM_15, GPIO_NUM_4, SSD1306_128x64, 0x3c );
    if ( oled.init() ) {
        std::cout << "OLED initialized\n";
        oled.clear();
        oled.select_font( 1 );
        oled.draw_string( 0, 0, "Hi, OLED!", WHITE, BLACK );
        oled.refresh( true );
    } else {
        std::cout << "OLED did not initialize\n";
    } 
    return 0;
}
