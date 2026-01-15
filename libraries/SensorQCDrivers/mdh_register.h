#ifndef SPIFFS_SENSOR_H
#define SPIFFS_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"

typedef struct mdh_register_init_options_t
{
} mdh_register_init_options_t;

typedef struct mdh_register_data_t
{
} mdh_register_data_t;

void mdh_register_data_destroy(void *);

class MDHRegister : public SensorBase
{
public:
  MDHRegister();
  ~MDHRegister();
  bool init(void *args);
  void read(void *data);
  String toJson(void *data);
  String asRAW(void *data);
  void *newData();
  String toCSV(void *data);
  void newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data);
  void printData(void *data);
  void handleApi(AsyncWebServerRequest *request, JsonVariant &json);
  bool registerApi();
  bool isEnabled();
  int getDataSize();
};

#endif
