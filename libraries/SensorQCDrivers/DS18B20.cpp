/*************************************************************************
 Title  :   C++ file include functions to load SPIFFS file content (ble.cpp)
 Author:    Subhasish Das <sudas@machinesense.com>

***************************************************************************/

#include <Arduino.h>
#include <ArduinoJson.h>
#include <DallasTemperature.h> //Temperature sensor library
#include <OneWire.h>           // Library for one wire communicaion
#include "DS18B20.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void lt_data_destroy(void *data)
{
}

LTSensor::LTSensor()
{
  data_delete_func = lt_data_destroy;
  class_name = "LTSensor";
}

LTSensor::~LTSensor()
{
  delete (one_wire);
  delete (sensor_dallas);
}

bool LTSensor::init(void *args)
{
  lt_sensor_init_options_t *opts = (lt_sensor_init_options_t *)args;
  ESP_LOGD(TAG, "Initializing LT Sensor at Pin: %d ", opts->pin_number);
  one_wire = new OneWire(opts->pin_number);
  sensor_dallas = new DallasTemperature(one_wire);
  sensor_dallas->begin();
  sensor_dallas->setResolution(9);
  sensor_dallas->setWaitForConversion(false);
  ESP_LOGD(TAG, "LT Sensor Initialized!\n");
}

void LTSensor::read(void *data)
{
  lt_data_t *lt_data = (lt_data_t *)data;
  sensor_dallas->requestTemperatures();
  uint8_t temp = sensor_dallas->getTempCByIndex(0);
  if ((temp > (-55.00)) && (temp < 125.00) && (temp != 0.0)) // checking for garbage value
  {
    goodRead();
  }
  else
  {
    failedRead();
    ESP_LOGD(TAG, "LT Sensor: Read Error!!\n");
    return;
  }
  lt_data->temperature = temp;
}

void LTSensor::printData(void *data)
{
  lt_data_t *lt_data = (lt_data_t *)data;
  Serial.print("Temperture: ");
  Serial.println(String(lt_data->temperature));
}

void *LTSensor::newData()
{
  return calloc(1, sizeof(lt_data_t));
}

void LTSensor::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(lt_sensor_init_options_t));
  lt_sensor_init_options_t *lt_sensor_init_options = (lt_sensor_init_options_t *)*sensor_init_opts;
  if (!lt_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating DHT Sensor options!\n");
    return;
  }
  lt_sensor_init_options->pin_number = (uint8_t)json_data["pin_number"].as<unsigned short>();
}

String LTSensor::toJson(void *data)
{
  lt_data_t *lt_data = (lt_data_t *)data;
  String json_str = "{";
  json_str += jsonStringVal("temperature") + ":" + jsonStringVal((unsigned int)lt_data->temperature);
  json_str += "}";
  return json_str;
}

String LTSensor::toCSV(void *data)
{
  lt_data_t *lt_data = (lt_data_t *)data;
  String csv;

  csv += String(lt_data->temperature);
  return csv;
}

void LTSensor::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool LTSensor::registerApi()
{
  return false;
}

bool LTSensor::isEnabled()
{
  return true;
}

int LTSensor::getDataSize()
{
  return sizeof(lt_data_t);
}