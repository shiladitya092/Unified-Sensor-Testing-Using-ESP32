#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include "mdh_register.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void mdh_register_data_destroy(void *)
{
}

MDHRegister::MDHRegister()
{
  data_delete_func = mdh_register_data_destroy;
  class_name = "MDHRegister";
}

MDHRegister::~MDHRegister()
{
}

bool MDHRegister::init(void *args)
{
  ESP_LOGD(TAG, "Initializing MDHRegister sensor...\n");
  ESP_LOGD(TAG, "MDHRegister Sensor Initialized!\n");
}

void MDHRegister::read(void *data)
{
  goodRead();
}

String MDHRegister::toJson(void *data)
{
  String json_str = "{}";
  return json_str;
}

String MDHRegister::asRAW(void *data)
{
  String json_str = "{";
  json_str += jsonStringVal("macid") + ":" + jsonStringVal(String(WiFi.macAddress())) + ",";
  json_str += jsonStringVal("ipaddress") + ":" + jsonStringVal(WiFi.localIP().toString());
  json_str += "}";
  return json_str;
}

String MDHRegister::toCSV(void *data)
{
  String csv = "";

  csv += jsonStringVal(String(WiFi.macAddress())) + "," + jsonStringVal(WiFi.localIP().toString());
  return csv;
}

void *MDHRegister::newData()
{
  mdh_register_data_t *tmp = (mdh_register_data_t *)calloc(1, sizeof(mdh_register_data_t));
  return tmp;
}

void MDHRegister::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(mdh_register_init_options_t));
  mdh_register_init_options_t *mdh_register_sensor_init_options = (mdh_register_init_options_t *)*sensor_init_opts;
  if (!mdh_register_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating SPIFFS Sensor options!\n");
    return;
  }
}

void MDHRegister::printData(void *data)
{
}

void MDHRegister::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool MDHRegister::registerApi()
{
  return false;
}

bool MDHRegister::isEnabled()
{
  return true;
}

int MDHRegister::getDataSize()
{
  return sizeof(mdh_register_data_t);
}