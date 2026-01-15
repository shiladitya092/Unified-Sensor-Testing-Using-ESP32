#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
//#include <SoftwareSerial.h>
#include "DHTSensor.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void dht_data_destroy(void *data)
{
}

DHTSensor::DHTSensor()
{
  data_delete_func = dht_data_destroy;
  class_name = "DHTSensor";
}

DHTSensor::~DHTSensor()
{
  delete (dht);
}

bool DHTSensor::init(void *args)
{
  dht_sensor_init_options_t *opts = (dht_sensor_init_options_t *)args;
  ESP_LOGD(TAG, "Initializing DHT Sensor at Pin: %d \n", opts->dht_pin);
  dht = new DHT(opts->dht_pin, DHTTYPE);
  dht->begin();
  ESP_LOGD(TAG, "DHT Sensor Initialized!\n");
}

void DHTSensor::read(void *data)
{
  dht_data_t *dht_data = (dht_data_t *)data;
  float a, b;
  a = dht->readTemperature();
  b = dht->readHumidity();
  if (!isnan(a) && !isnan(b))
  {
    goodRead();
  }
  else
  {
    ESP_LOGD(TAG, "DHT Sensor: Read Error!!\n");
    failedRead();
    return;
  }
  dht_data->temperature = a;
  dht_data->humidity = b;
}

String DHTSensor::toJson(void *data)
{
  dht_data_t *dht_data = (dht_data_t *)data;
  String json_str = "{";
  json_str += jsonStringVal("temperature") + ":" + jsonStringVal((unsigned int)dht_data->temperature);
  json_str += ",";
  json_str += jsonStringVal("humidity") + ":" + jsonStringVal((unsigned int)dht_data->humidity);
  json_str += "}";
  return json_str;
}

String DHTSensor::toCSV(void *data)
{
  dht_data_t *dht_data = (dht_data_t *)data;
  String csv = "";

  csv += String(dht_data->temperature) + ",";
  csv += String(dht_data->humidity);
  return csv;
}

void *DHTSensor::newData()
{
  dht_data_t *tmp = (dht_data_t *)calloc(1, sizeof(dht_data_t));
  return tmp;
}

void DHTSensor::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(dht_sensor_init_options_t));
  dht_sensor_init_options_t *dht_sensor_init_options = (dht_sensor_init_options_t *)*sensor_init_opts;
  if (!dht_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating DHT Sensor options!\n");
    return;
  }
  dht_sensor_init_options->dht_pin = (uint8_t)json_data["dht_pin"].as<unsigned short>();
}

void DHTSensor::printData(void *data)
{
  dht_data_t *dht_data = (dht_data_t *)data;
  Serial.print("Temperture: ");
  Serial.print(String(dht_data->temperature));
  Serial.print(", Humidity: ");
  Serial.println(String(dht_data->humidity));
}

void DHTSensor::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool DHTSensor::registerApi()
{
  return false;
}

bool DHTSensor::isEnabled()
{
  return true;
}

int DHTSensor::getDataSize()
{
  return sizeof(dht_data_t);
}