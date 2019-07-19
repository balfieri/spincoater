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
#include "mpu9255.h"            // motion and temperature sensor
#include "driver/adc.h"         // analog inputs

int main()
{
    // I2C OLED
    //             RST          SCL          SDA       Resolution    I2C Addr
    OLED oled( GPIO_NUM_16, GPIO_NUM_15, GPIO_NUM_4, SSD1306_128x64, 0x3c );
    if ( oled.init() ) {
        oled.clear();
        oled.select_font( 1 );
        oled.draw_string( 0, 0, "Hi, OLED!", WHITE, BLACK );
        oled.refresh( true );
    } 

    // SPI sensors     CS          SCLK        MISI          MISO
    MAX6675 temp( GPIO_NUM_13, GPIO_NUM_18,              GPIO_NUM_19 );
    MPU9255 mpu(  GPIO_NUM_5,  GPIO_NUM_18, GPIO_NUM_23, GPIO_NUM_19 );

    // ADC sensors
    adc1_config_width( ADC_WIDTH_BIT_12 );
    adc1_config_channel_atten( ADC1_CHANNEL_0, ADC_ATTEN_DB_0 );

    for( ;; )
    {
        std::cout << "\n";

        double C = temp.readC();
        double F = 32.0 + 9.0/5.0 * C;
        std::cout << "Temperature: ";
        if ( __isnand(C) ) {
            std::cout << "no thermocouple attached!\n";
        } else {
            std::cout << C << "C (" << F << "F)\n";
        }

        MPU9255::Raw_Info raw;
        mpu.raw_read( raw );
        std::cout << "Raw Accel:   [" << raw.accel[0] << ", " << raw.accel[1] << ", " << raw.accel[2] << "]\n";
        std::cout << "Raw Temp:    " << raw.temp << "\n";
        std::cout << "Raw Gyro:    [" << raw.gyro[0] << ", " << raw.gyro[1] << ", " << raw.gyro[2] << "]\n";

        int32_t sound = adc1_get_raw( ADC1_CHANNEL_6 );
        std::cout << "Raw Sound:   " << sound << "\n";

        int32_t hall = hall_sensor_read();
        std::cout << "Raw Hall:    " << hall << "\n";

        ets_delay_us( 1000000 );
    }

    return 0;
}
