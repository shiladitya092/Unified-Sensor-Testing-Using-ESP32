#ifndef ADS7844_H
#define ADS7844_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"

typedef struct ads7844_init_options_t
{
  uint8_t cs_pin;
  int iterations;
} ads7844_init_options_t;

typedef struct ads7844_data_t
{
  double average;
  double std_dev;
  double clean_avg;
  int clean_count;

} ads7844_data_t;

void ads7844_data_destroy(void *data);

class ADS7844 : public SensorBase
{
private:
  uint8_t cs_pin;
  int iterations;
  float read_adc();
  void calculate_meta(float *data, int count, ads7844_data_t *output);
  String toJson(void *data);

public:
  ADS7844();
  ~ADS7844();
  bool init(void *args);
  void read(void *data);
  String toJson(void **data, uint16_t count);
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
