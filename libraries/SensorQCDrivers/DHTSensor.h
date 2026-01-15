#ifndef DHTSENSOR_H
#define DHTSENSOR_H

//#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "sensor_base.h"
#include "QuickMedianLib.h"

#define DHTTYPE DHT22

typedef struct dht_sensor_init_options_t
{
  uint8_t dht_pin;

} dht_sensor_init_options_t;

typedef struct dht_data_t
{
  uint8_t humidity;
  uint8_t temperature;

} dht_data_t;

void dht_data_destroy(void *data);

class DHTSensor : public SensorBase
{
private:
  DHT *dht;

public:
  DHTSensor();
  ~DHTSensor();
  bool init(void *args);
  void read(void *data);
  String toJson(void *data);
  String toCSV(void *data);
  void *newData();
  void newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data);
  void printData(void *data);
  void handleApi(AsyncWebServerRequest *request, JsonVariant &json);
  bool registerApi();
  bool isEnabled();
  int getDataSize();
};

#endif
