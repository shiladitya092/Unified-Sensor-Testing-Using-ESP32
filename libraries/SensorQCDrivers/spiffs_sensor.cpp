#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include "spiffs_sensor.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void spiffs_data_destroy(void *)
{
}

SPIFFSSensor::SPIFFSSensor()
{
  data_delete_func = spiffs_data_destroy;
  class_name = "SPIFFSSensor";
}

SPIFFSSensor::~SPIFFSSensor()
{
}

bool SPIFFSSensor::init(void *args)
{
  spiffs_sensor_init_options_t *opts = (spiffs_sensor_init_options_t *)args;
  ESP_LOGD(TAG, "Initializing SPIFFS sensor...\n");
  led_pin = opts->led_pin;
  threshold = opts->threshold;
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  ESP_LOGD(TAG, "SPIFFS Sensor Initialized!\n");
}

void SPIFFSSensor::read(void *data)
{
  spiffs_data_t *spiffs_data = (spiffs_data_t *)data;
  spiffs_data->usage = (float)SPIFFS.usedBytes() / SPIFFS.totalBytes();
  // Serial.printf("Usage: %f\n", spiffs_data->usage);
  goodRead();
  if (spiffs_data->usage > threshold)
  {
    digitalWrite(led_pin, HIGH);
  }
  else
  {
    digitalWrite(led_pin, LOW);
  }
}

String SPIFFSSensor::toJson(void *data)
{
  spiffs_data_t *spiffs_data = (spiffs_data_t *)data;
  String json_str = "{";
  json_str += jsonStringVal("usage") + ":" + jsonStringVal(spiffs_data->usage);
  json_str += "}";
  return json_str;
}

String SPIFFSSensor::toCSV(void *data)
{
  spiffs_data_t *spiffs_data = (spiffs_data_t *)data;
  String csv = "";

  csv += String(spiffs_data->usage);
  return csv;
}

void *SPIFFSSensor::newData()
{
  spiffs_data_t *tmp = (spiffs_data_t *)calloc(1, sizeof(spiffs_data_t));
  return tmp;
}

void SPIFFSSensor::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(spiffs_sensor_init_options_t));
  spiffs_sensor_init_options_t *spiffs_sensor_init_options = (spiffs_sensor_init_options_t *)*sensor_init_opts;
  if (!spiffs_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating SPIFFS Sensor options!\n");
    return;
  }
  spiffs_sensor_init_options->threshold = json_data["threshold"].as<float>();
  spiffs_sensor_init_options->led_pin = (uint8_t)json_data["led_pin"].as<unsigned short>();
}

void SPIFFSSensor::printData(void *data)
{
  spiffs_data_t *spiffs_data = (spiffs_data_t *)data;
  ESP_LOGD(TAG, "Usage: ");
  ESP_LOGD(TAG, "%f", spiffs_data->usage);
  ESP_LOGD(TAG, "\n");
}

void SPIFFSSensor::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool SPIFFSSensor::registerApi()
{
  return false;
}

bool SPIFFSSensor::isEnabled()
{
  return true;
}

int SPIFFSSensor::getDataSize()
{
  return sizeof(spiffs_data_t);
}