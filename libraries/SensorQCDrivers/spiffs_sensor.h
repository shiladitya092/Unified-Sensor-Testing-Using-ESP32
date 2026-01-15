#ifndef SPIFFS_SENSOR_H
#define SPIFFS_SENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"

typedef struct spiffs_sensor_init_options_t
{
  uint8_t led_pin;
  float threshold;
} spiffs_sensor_init_options_t;

typedef struct spiffs_data_t
{
  float usage;
} spiffs_data_t;

void spiffs_data_destroy(void *);

class SPIFFSSensor : public SensorBase
{
private:
  uint8_t led_pin;
  float threshold;

public:
  SPIFFSSensor();
  ~SPIFFSSensor();
  bool init(void *args);
  void read(void *data);
  String toJson(void *data);
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
