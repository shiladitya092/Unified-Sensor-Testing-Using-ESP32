#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include "configurator.h"

#define MAX_CONFIG_JSON_SIZE 8192



Configurator global_configurator;
Configurator wifi_configurator;

Configurator::Configurator() : config_json(MAX_CONFIG_JSON_SIZE)
{
}
Configurator::~Configurator()
{
}

void Configurator::init(String config_file)
{
    this->config_file = String(config_file);
}

bool Configurator::loadConfigs()
{
    try
    {
        DeserializationError error;
        File file = SPIFFS.open(config_file, FILE_READ);
        if (!file)
        {
            ESP_LOGD(TAG, "Failed to open file: %s\n", config_file.c_str());
            return false;
        }
        error = deserializeJson(config_json, file);
        file.close();
        if (error)
        {
            ESP_LOGD(TAG, "DeserializeJson() failed with code: ");
            ESP_LOGD(TAG, "%s", error.f_str());
            return false;
        }
        else
        {
            serializeJsonPretty(config_json, Serial);
            Serial.println("");
            return true;
        }
    }
    catch (...)
    {
        ESP_LOGD(TAG, "An error occurred while reading config file!\n");
        return false;
    }
}

bool Configurator::saveConfigs()
{
    try
    {
        File file = SPIFFS.open(config_file, FILE_WRITE);
        if (!file)
        {
            ESP_LOGD(TAG, "Failed to open file: %s\n", config_file.c_str());
            return false;
        }
        serializeJson(config_json, file);
        file.close();
        return true;
    }
    catch (...)
    {
        ESP_LOGD(TAG, "An error occurred while wirting to config file!\n");
        return false;
    }
}

bool merge(JsonVariant dst, JsonVariant src)
{
    try
    {
        if (src.is<JsonObject>())
        {
            for (auto kvp : src.as<JsonObject>())
            {
                merge(dst.getOrAddMember(kvp.key()), kvp.value());
            }
        }
        else
        {
            dst.set(src);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool Configurator::mergeConfig(String new_config_string)
{
    DynamicJsonDocument new_config(MAX_CONFIG_JSON_SIZE);

    DeserializationError error;
    error = deserializeJson(new_config, new_config_string);
    if (error)
    {
        ESP_LOGD(TAG, "DeserializeJson() failed with code: ");
        ESP_LOGD(TAG, "%s", error.f_str());
        ESP_LOGD(TAG, "\n");
        return false;
    }
    else
    {
        ESP_LOGD(TAG, "\n");
        if (merge(config_json.as<JsonVariant>(), new_config.as<JsonVariant>()))
        {
            config_json.garbageCollect();
            ESP_LOGD(TAG, "New Config:\n");
            serializeJsonPretty(config_json, Serial);
            Serial.println("");
            return true;
        }
        else
            return false;
    }
}
