//-----------------------------------------------------
// WiFi Utility Class
//-----------------------------------------------------

#ifndef WIFI_H_
#define WIFI_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

class WiFi
{
public:
    static void init( const char * ssid, const char * password, uint32_t max_retries );

private:
    static esp_err_t event_handler( void *ctx, system_event_t *event );
    static void      str_copy8( uint8_t * s8, const char * s, uint32_t n );
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
static uint32_t           wifi_max_retries = 1;
static uint32_t           wifi_num_retries = 0;
static EventGroupHandle_t wifi_event_group;

void WiFi::init( const char * ssid, const char * password, uint32_t max_retries )
{
    wifi_max_retries = max_retries;
    wifi_num_retries = 0;
    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    esp_event_loop_init(event_handler, NULL);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    wifi_config_t wifi_config;
    str_copy8( wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) );
    str_copy8( wifi_config.sta.password, password, sizeof(wifi_config.sta.password) );

    ESP_LOGI("WiFi", "Joining %s...", ssid );
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    if ( !esp_wifi_start() ) {
        ESP_LOGE( "WiFi", "Could not join %s", ssid );
    }
}

esp_err_t WiFi::event_handler( void *ctx, system_event_t *event )
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        wifi_num_retries = 0;
        xEventGroupSetBits(wifi_event_group, BIT0);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI( "WiFi", "connection to AP failed" );
        if ( wifi_num_retries < wifi_max_retries ) {
            esp_wifi_connect();
            xEventGroupClearBits(wifi_event_group, BIT0);
            wifi_num_retries++;
            ESP_LOGI( "WiFi", "retried connection to AP" );
        }
        break;
    default:
        break;
    }
    return ESP_OK;
}

void WiFi::str_copy8( uint8_t * s8, const char * s, uint32_t n )
{
    for( uint32_t i = 0; i < n; i++ )
    {
        s8[i] = s[i];
        if ( s8[i] == 0 ) break;
    }
}

#endif // WIFI_H_
