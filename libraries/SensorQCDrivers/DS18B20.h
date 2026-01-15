#ifndef DS18B20_H
#define DS18B20_H

#include <DallasTemperature.h> //Temperature sensor library
#include <OneWire.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <QuickMedianLib.h>
#include "sensor_base.h"

typedef struct lt_sensor_init_options_t
{
  uint8_t pin_number;
} lt_sensor_init_options_t;

typedef struct lt_data_t
{
  uint8_t temperature;
} lt_data_t;

void lt_data_destroy(void *data);

class LTSensor : public SensorBase
{
private:
  OneWire *one_wire;
  DallasTemperature *sensor_dallas;

public:
  LTSensor();
  ~LTSensor();
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
