#define DYNAMIC_JSON_DOCUMENT_SIZE 8192
#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "config_api.h"
#include "web_helpers.h"
#include "qc_helpers.h"
#include "configurator.h"
#include "system_tools.h"
#include "buildinfo.h"
#include "controller_container.h"

// static AsyncWebServer *webSvr_handle;
// StaticJsonDocument<256> json_data;

void config_api_upload(Configurator &configurator, AsyncWebServerRequest *request, JsonVariant &json)
{
    if (true)
    {
        String new_config_string = json.as<String>();
        if (configurator.mergeConfig(new_config_string))
        {
            if (configurator.saveConfigs())
            {
                send_json_success_response(request, jsonStringVal("Config saved successfully! Restarting..."));
            }
            else
            {
                send_json_error_response(request, "Could not save the new configs!");
            }
            espRestart();
        }
        else
        {
            send_json_error_response(request, "An error occurred while merging the config!");
        }
    }
    else
    {
        send_json_error_response(request, "Config could not be found!");
    }
}

void config_api_upload_global(AsyncWebServerRequest *request, JsonVariant &json)
{
    config_api_upload(global_configurator, request, json);
}

void config_api_upload_wifi(AsyncWebServerRequest *request, JsonVariant &json)
{
    config_api_upload(wifi_configurator, request, json);
}

void build_info(AsyncWebServerRequest *request)
{
    String json = "{";

    json += jsonStringVal("data") + ":" + jsonStringVal(_BuildInfo.date) + ",";
    json += jsonStringVal("time") + ":" + jsonStringVal(_BuildInfo.time) + ",";
    json += jsonStringVal("src_version") + ":" + jsonStringVal(_BuildInfo.src_version) + ",";
    json += jsonStringVal("env_version") + ":" + jsonStringVal(_BuildInfo.env_version);
    json += "}";
    send_json_success_response(request, json);
}

void register_config_api(AsyncWebServer &webSvr_handle)
{
    // webSvr_handle.on("/config/upload_global", config_api_upload_global);
    AsyncCallbackJsonWebHandler* config_api_upload_global_handler = new AsyncCallbackJsonWebHandler("/config/upload_global", config_api_upload_global);
    webSvr_handle.addHandler(config_api_upload_global_handler);
    // webSvr_handle.on("/config/upload_wifi", config_api_upload_wifi);
    AsyncCallbackJsonWebHandler* config_api_upload_wifi_handler = new AsyncCallbackJsonWebHandler("/config/upload_wifi", config_api_upload_wifi);
    webSvr_handle.addHandler(config_api_upload_wifi_handler);
    webSvr_handle.on("/system/build", build_info);
}