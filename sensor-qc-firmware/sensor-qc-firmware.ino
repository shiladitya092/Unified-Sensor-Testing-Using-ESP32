#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <string.h>
#include "configurator.h"
#include "system_loader.h"
#include "controller_container.h"
#include "spiffs_api.h"
#include "config_api.h"
#include "system_tools.h"
#include "time_operations.h"
#include "sensor_base.h"
#include "user_interface.h"

#define WEBSERVER_PORT 80
#define VERSION 199.1001

#define TAG "UDAM"

AsyncWebServer server(WEBSERVER_PORT);
AsyncWebSocket ws("/ws");
AsyncWebSocketClient *global_ws_client = NULL;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    ESP_LOGD(TAG, "Socket Event");
    if (type == WS_EVT_CONNECT)
    {
        ESP_LOGD(TAG, "Websocket client connection received");
        global_ws_client = client;
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        ESP_LOGD(TAG, "Client disconnected");
        global_ws_client = NULL;
    }
}

void wifi_ap_setup()
{
    String wifi_mac = String(WiFi.macAddress());
    WiFi.disconnect();
    WiFi.mode(WIFI_AP);
    String hotspot_prefix = wifi_configurator.config_json["hotspot_prefix"].as<String>();
    String ssid1 = "VW_TEST_" + wifi_mac;
    if (hotspot_prefix != "null")
        ssid1 = hotspot_prefix + wifi_mac;
    WiFi.softAP((const char *)ssid1.c_str(), (const char *)wifi_mac.c_str());
}

void wifi_setup()
{
    String wifi_mac = String(WiFi.macAddress());
    const char *ssid = wifi_configurator.config_json["ssid"].as<char *>();
    const char *password = wifi_configurator.config_json["password"].as<char *>();
    int retries = 0;
    int max_tries = 120;
    WiFi.begin(ssid, password);
    Serial.print("Connecting\n");
    if (strlen(ssid) > 0 && strlen(password) >= 8)
    {
        while (WiFi.status() != WL_CONNECTED && (retries < max_tries || max_tries == 0))
        {
            delay(500);
            Serial.print(".");
            retries++;
        }
    }
    else
    {
        Serial.println("WiFi SSID or PASSWORD not provided or is incorrectly set!");
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.printf("\nConnected to WiFi network with IP Address: %s\n\n", WiFi.localIP().toString().c_str());
        int rssi = 0;
        for (int i = 0; i < 10; i++)
        {
            rssi += WiFi.RSSI();
            delay(100);
        }
        if (rssi / 10.0 < -67)
        {
            Serial.printf("WiFi strength is too weak: %d! Disconnecting and enabling hotspot...", rssi / 10.0);
            wifi_ap_setup();
        }
    }
    else
    {
        Serial.print("Could not connect to WiFi! Enabling hotspot!\n");
        wifi_ap_setup();
    }
}

void enableCors()
{
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
}

void routeNotFound(AsyncWebServerRequest *request)
{
    if (request->method() == HTTP_OPTIONS)
    {
        request->send(200);
    }
    else
    {
        request->send(404);
    }
}

void server_setup()
{
    register_spiffs_api(server);
    register_config_api(server);
    register_ui_api(server);
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    enableCors();
    server.onNotFound(routeNotFound);
    server.begin();
}

void setup()
{
    Serial.begin(115200);
    SPIFFS.begin(true);
    wifi_configurator.init("/wifi_configs.json");
    wifi_configurator.loadConfigs();
    wifi_setup();
    global_configurator.init("/configs.json");
    global_configurator.loadConfigs();
    system_loader.createSenderContainer();
    system_loader.createControllerContainer();
    system_loader.setupMdhDiscoveryClient();
    setup_time();
    server_setup();
    ESP_LOGD(TAG, "Initialization Completed!\n");
}

void loop()
{
    if (keep_running)
    {
        global_container.loop();
        global_sender_container.loop();
    }
    delay(1);
    if (millis() % 5000 == 0)
    {
        ESP_LOGD(TAG, "%llu\n", get_soft_time_ms());
        Serial.printf("%llu\n", get_soft_time_ms());
        ESP_LOGD(TAG, "%d", ESP.getFreeHeap());
    }
}