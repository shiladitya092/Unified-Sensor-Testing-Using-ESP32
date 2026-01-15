#include <Arduino.h>
//#include <DHT.h>
//#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "ProbeSensor.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void probe_data_destory(void *)
{
}

ProbeSensor::ProbeSensor()
{
  data_delete_func = probe_data_destory;
  cycle_period_ns = 1000 / ESP.getCpuFreqMHz();
  class_name = "ProbeSensor";
}

ProbeSensor::~ProbeSensor()
{
}

bool ProbeSensor::init(void *args)
{
  probe_sensor_init_options_t *opts = (probe_sensor_init_options_t *)args;
  ESP_LOGD(TAG, "Initializing Probe Sensor at Pin: %d", opts->probe_pin);
  ESP_LOGD(TAG, "\n");
  this->pin = opts->probe_pin;
  this->control_enabled = opts->control_enabled;
  this->control_pin = opts->control_pin;
  this->enable_logic_level = opts->enable_logic_level;
  this->post_on_interval = opts->post_on_interval;
  this->post_off_interval = opts->post_off_interval;
  pinMode(pin, INPUT);
  if (control_enabled)
  {
    pinMode(control_pin, OUTPUT);
    digitalWrite(control_pin, LOW);
  }
  ESP_LOGD(TAG, "Probe Sensor Initialized!\n");
}

void ProbeSensor::read(void *data)
{
  probe_data_t *probe_data = (probe_data_t *)data;
  volatile uint32_t t1, t2;
  volatile unsigned long st;

  if (control_enabled)
  {
    digitalWrite(control_pin, enable_logic_level);
    if (post_on_interval > 0)
      delay(post_on_interval);
  }
  st = micros();
  while (digitalRead(pin) == LOW && micros() - st < 20000)
  {
  }
  while (digitalRead(pin) == HIGH && micros() - st < 40000)
  {
  }
  t1 = ESP.getCycleCount();
  while (digitalRead(pin) == LOW && micros() - st < 60000)
  {
  }
  while (digitalRead(pin) == HIGH && micros() - st < 80000)
  {
  }
  t2 = ESP.getCycleCount();
  if (control_enabled)
  {
    int logic_level = enable_logic_level == 0 ? 1 : 0;
    digitalWrite(control_pin, logic_level);
    if (post_off_interval > 0)
      delay(post_off_interval);
  }
  double period = ((t2 - t1) * cycle_period_ns);
  if (period <= 0 || period >= 40000000)
  {
    probe_data->frequency = 0;
    ESP_LOGD(TAG, "Probe Sensor: Read Error!!\n");
    failedRead();
  }
  else
  {
    probe_data->frequency = (1000000000.0 / period);
    goodRead();
  }
}

String ProbeSensor::toJson(void *data)
{
  probe_data_t *probe_data = (probe_data_t *)data;
  String json_str = "{";
  json_str += jsonStringVal("frequency") + ":" + jsonStringVal(probe_data->frequency);
  json_str += "}";
  return json_str;
}

String ProbeSensor::toCSV(void *data)
{
  probe_data_t *probe_data = (probe_data_t *)data;
  String csv = "";
  csv += String(probe_data->frequency, 4);
  return csv;
}

void *ProbeSensor::newData()
{
  probe_data_t *tmp = (probe_data_t *)calloc(1, sizeof(probe_data_t));

  return tmp;
}

void ProbeSensor::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(probe_sensor_init_options_t));
  probe_sensor_init_options_t *probe_sensor_init_options = (probe_sensor_init_options_t *)*sensor_init_opts;
  if (!probe_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating Probe Sensor options!\n");
    return;
  }
  probe_sensor_init_options->probe_pin = (uint8_t)json_data["probe_pin"].as<unsigned short>();
  probe_sensor_init_options->control_enabled = json_data["control_enabled"].as<bool>();
  probe_sensor_init_options->control_pin = (uint8_t)json_data["control_pin"].as<unsigned short>();
  probe_sensor_init_options->enable_logic_level = (uint8_t)json_data["enable_logic_level"].as<unsigned short>();
  probe_sensor_init_options->post_on_interval = json_data["post_on_interval"].as<unsigned long>();
  probe_sensor_init_options->post_off_interval = json_data["post_off_interval"].as<unsigned long>();
}

void ProbeSensor::printData(void *data)
{
  probe_data_t *probe_data = (probe_data_t *)data;
  Serial.print("Frequency: ");
  Serial.println(String(probe_data->frequency));
}

void ProbeSensor::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool ProbeSensor::registerApi()
{
  return false;
}

bool ProbeSensor::isEnabled()
{
  return true;
}

int ProbeSensor::getDataSize()
{
  return sizeof(probe_data_t);
}