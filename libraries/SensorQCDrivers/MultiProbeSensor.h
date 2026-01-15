#ifndef MULTIPROBESENSOR_H
#define MULTIPROBESENSOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"
#include "ProbeSensor.h"

typedef enum push_button_mode_t
{
  PUSH_BUTTON_MODE_HARDWARE = 0,
  PUSH_BUTTON_MODE_SOFTWARE,
  PUSH_BUTTON_MODE_ALWAYS_ON
} push_button_mode_t;

typedef struct multi_probe_sensor_init_options_t
{
  probe_sensor_init_options_t **probe_sensor_opts_list;
  int num_probe;
  int push_button_pin;
  push_button_mode_t push_button_mode;
} multi_probe_sensor_init_options_t;

typedef struct multi_probe_data_t
{
  probe_data_t *probe_data_list;
  int count;
  int push_button_state;
} multi_probe_data_t;

void multi_probe_data_destroy(void *data);

class MultiProbeSensor : public SensorBase
{
private:
  int num_probe;
  int push_button_pin;
  push_button_mode_t push_button_mode;
  uint8_t push_button_value;
  ProbeSensor *probe_list;
  String process_control(const String &response);

public:
  MultiProbeSensor();
  ~MultiProbeSensor();
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
  void setLastData(void *new_data);
};

#endif
