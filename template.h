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

#include "delay.h"              
#include "nvs_flash.h"
#include "wifi.h"
#include "math.h"
#include "oled.h"
#include "max6675.h"            // temperature sensor
#include "mpu9255.h"            // motion and temperature sensor
#include "driver/adc.h"         // analog inputs

int main()
{
    // OVEN RELAY   
    int relay_n = 1; 
    gpio_num_t relay_pin = GPIO_NUM_25;
    gpio_set_direction( relay_pin, GPIO_MODE_OUTPUT );
    gpio_set_level( relay_pin, relay_n );

    // NVS
    esp_err_t ret = nvs_flash_init();
    if ( ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND ) {
      ESP_ERROR_CHECK( nvs_flash_erase() );
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    // WIFI
    WiFi::init( CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD, CONFIG_WIFI_MAX_RETRIES );

#if 0 
    // I2C OLED
    //             RST          SCL          SDA       Resolution    I2C Addr
    OLED oled( GPIO_NUM_23, GPIO_NUM_22, GPIO_NUM_21, SSD1306_128x64, 0x3c );
    if ( oled.init() ) {
        oled.clear();
        oled.select_font( 1 );
        oled.draw_string( 0, 0, "Hi, OLED!", WHITE, BLACK );
        oled.refresh( true );
    } 
#endif

    // SPI sensors     CS          SCLK        MISI          MISO
    MAX6675 temp( GPIO_NUM_5, GPIO_NUM_18,              GPIO_NUM_19 );

#if 0
    MPU9255 mpu(  GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_23, GPIO_NUM_19 );

    // ADC sensors
    adc1_config_width( ADC_WIDTH_BIT_12 );
    adc1_config_channel_atten( ADC1_CHANNEL_6, ADC_ATTEN_DB_0 );
#endif

    double C_max = 0;
    for( ;; )
    {
        Delay::sec( 1 );
        std::cout << "\n";

        double C = temp.readC();
        double F = 32.0 + 9.0/5.0 * C;
        if ( C > C_max ) C_max = C;
        std::cout << "Current:  ";
        if ( __isnand(C) ) {
            std::cout << "no thermocouple attached!\n";
        } else {
            std::cout << C << "C  (" << F << "F)\n";
        }
        double F_max = 32.0 + 9.0/5.0 * C_max; 
        std::cout << "Max Seen: " << C_max << "C  (" << F_max << "F)\n";

        double C_t = 150;
        double F_t = 32.0 + 9.0/5.0 * C_t;
        std::cout << "Target:   " << C_t << "C  (" << F_t << "F)\n";

        relay_n = C >= 150;
        std::cout << "Relay:    " << (relay_n ? "OFF" : "ON") << "\n";
        gpio_set_level( relay_pin, relay_n );

#if 0
        MPU9255::Raw_Info raw;
        mpu.raw_read( raw );
        C = mpu.raw_temp_to_C( raw.temp );
        F = 32.0 + 9.0/5.0 * C;
        std::cout << "Raw Accel:   [" << raw.accel[0] << ", " << raw.accel[1] << ", " << raw.accel[2] << "]\n";
        std::cout << "Raw Temp:    " << raw.temp << " (" << C << "C, " << F << "F)\n";
        std::cout << "Raw Gyro:    [" << raw.gyro[0] << ", " << raw.gyro[1] << ", " << raw.gyro[2] << "]\n";

        int32_t sound = adc1_get_raw( ADC1_CHANNEL_6 );
        std::cout << "Raw Sound:   " << sound << "\n";

        int32_t hall = hall_sensor_read();
        std::cout << "Raw Hall:    " << hall << "\n";
#endif
    }

    return 0;
}
