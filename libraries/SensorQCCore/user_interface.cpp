#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <SPIFFS.h>
#include <FS.h>
#include "configurator.h"
#include "controller_container.h"

String sensor_routes_processor()
{
    int i;
    sensor_api_list_t sensor_api_list;
    String routes_html = "";

    global_container.listSensorApi(sensor_api_list);
    for (i = 0; i < sensor_api_list.count; i++)
    {
        routes_html += "<a href=/?sensor=" + sensor_api_list.routes[i] + ">" + sensor_api_list.routes[i] + "</a><br>\n";
    }
    return routes_html;
}

String index_processor(const String &var)
{
    if (var == "WIFI_SSID")
    {
        return String(wifi_configurator.config_json["ssid"].as<String>());
    }
    else if (var == "WIFI_PASSWORD")
    {
        return String(wifi_configurator.config_json["password"].as<String>());
    }
    else if (var == "HOTSPOT_PREFIX")
    {
        return String(wifi_configurator.config_json["hotspot_prefix"].as<String>());
    }
    else if (var == "SENSOR_ROUTES")
    {
        return sensor_routes_processor();
    }
}

void ui_index_json(AsyncWebServerRequest *request, JsonVariant &json)
{
    if (request->hasParam("sensor"))
    {
        AsyncWebParameter *p = request->getParam("sensor");
        String sensor_name = String(p->value());
        ESP_LOGD(TAG, "Sensor API Call Args: %s", sensor_name.c_str());
        global_container.handleApi(sensor_name, request, json);
    }
}

void ui_index(AsyncWebServerRequest *request)
{
    if (request->hasParam("sensor"))
    {
        JsonVariant json;
        AsyncWebParameter *p = request->getParam("sensor");
        String sensor_name = String(p->value());
        ESP_LOGD(TAG, "Sensor API Call Args: %s", sensor_name.c_str());
        global_container.handleApi(sensor_name, request, json);
    }
    else
    {
        request->send(SPIFFS, "/index.html", String(), false, index_processor);
    }
}

void register_ui_api(AsyncWebServer &webSvr_handle)
{
    AsyncCallbackJsonWebHandler *ui_index_handler = new AsyncCallbackJsonWebHandler("/", ui_index_json);
    webSvr_handle.addHandler(ui_index_handler);
    webSvr_handle.on("/", ui_index);
}
