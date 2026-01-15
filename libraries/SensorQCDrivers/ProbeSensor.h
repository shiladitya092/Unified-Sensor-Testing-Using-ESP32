#ifndef PROBESENSOR_H
#define PROBESENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"

typedef struct probe_sensor_init_options_t
{
  uint8_t probe_pin;
  bool control_enabled;
  uint8_t control_pin;
  uint8_t enable_logic_level;
  unsigned long post_on_interval;
  unsigned long post_off_interval;
} probe_sensor_init_options_t;

typedef struct probe_data_t
{
  double frequency;

} probe_data_t;

typedef struct freq_item
{
  unsigned long ontime;
  unsigned long offtime;
  float period;
  double conductivity;
} freq_item;

void probe_data_destory(void *);

class ProbeSensor : public SensorBase
{
private:
  uint8_t pin;
  bool control_enabled;
  uint8_t control_pin;
  uint8_t enable_logic_level;
  unsigned long post_on_interval;
  unsigned long post_off_interval;
  double cycle_period_ns;

public:
  ProbeSensor();
  ~ProbeSensor();
  String toJson(void *data);
  String toCSV(void *data);
  bool init(void *args);
  void read(void *data);
  void *newData();
  void newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data);
  void printData(void *data);
  void handleApi(AsyncWebServerRequest *request, JsonVariant &json);
  bool registerApi();
  bool isEnabled();
  int getDataSize();
};

#endif
