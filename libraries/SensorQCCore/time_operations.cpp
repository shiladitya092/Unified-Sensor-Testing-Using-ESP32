#include "time_operations.h"
#include <Arduino.h>
#include <time.h>
#include <NTPClient.h>
#include <WiFi.h>
#include "configurator.h"

time_data_t time_data;

void time_setup()
{
    String ntpServer = "0.in.pool.ntp.org";
    bool mdh_enabled = global_configurator.config_json["mdh_discovery_client"]["enabled"].as<bool>();
    if (mdh_enabled)
    {
        String mdh_server = global_configurator.config_json["mdh_discovery_client"]["mdh_server"].as<String>();
        String default_server = global_configurator.config_json["mdh_discovery_client"]["default_server"].as<String>();
        Serial.printf("MDH NTP Server: %s\n", mdh_server.c_str());
        Serial.printf("Default MDH NTP Server: %s\n", default_server.c_str());
        if (mdh_server.length() > 0 && mdh_server != "null")
            ntpServer = mdh_server;
        else if (default_server.length() > 0 && default_server != "null")
            ntpServer = default_server;
    }
    Serial.printf("NTP Server: %s\n", ntpServer.c_str());
    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 0;
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer.c_str());
    ESP_LOGD(TAG, "Attempted NTP sync!");
}

bool check_time_sync_status()
{
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        ESP_LOGD(TAG, "Failed local time...");
        return false;
    }
    ESP_LOGD(TAG, "OK local time...");
    return true;
}

void sync_ntp_time(TimerHandle_t pxTimer)
{
    if (!time_data.initialized)
    {
        time_setup();
        if (check_time_sync_status())
        {
            time_data.initialized = true;
            ESP_LOGD(TAG, "Updated time from NTP server!");
            xTimerStop(pxTimer, 0);
        }
        else
        {
            ESP_LOGD(TAG, "Unable to get time from NTP server!");
        }
    }
}

void setup_time()
{
    time_data.initialized = false;
    TimerHandle_t timer;
    timer = xTimerCreate("ntp_sync", 30000, pdTRUE, (void *)123, sync_ntp_time);
    sync_ntp_time(timer);
    xTimerStart(timer, 0);
}

unsigned long long get_soft_time_us()
{
    if (time_data.initialized)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long ts = (tv.tv_sec * 1000000ULL) + tv.tv_usec;
        return ts;
    }
    return micros();
}

unsigned long long get_soft_time_ms()
{
    if (time_data.initialized)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long ts = (tv.tv_sec * 1000ULL) + (tv.tv_usec / 1000ULL);
        return ts;
    }
    return millis();
}

unsigned long long get_soft_time_s()
{
    if (time_data.initialized)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        unsigned long long ts = tv.tv_sec + (tv.tv_usec / 1000000ULL);
        return ts;
    }
    return (millis() / 1000ULL);
}