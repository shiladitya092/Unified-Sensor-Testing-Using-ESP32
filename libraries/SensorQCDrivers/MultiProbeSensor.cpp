#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <cstdio>
#include <functional>
#include <ESPAsyncWebServer.h>
#include "MultiProbeSensor.h"
#include "qc_helpers.h"
#include "sensor_base.h"
#include "DHTSensor.h"
#include "controller_container.h"

extern AsyncWebSocketClient *global_ws_client;

void multi_probe_data_destroy(void *data)
{
  multi_probe_data_t *multi_probe_data = (multi_probe_data_t *)data;
  free(multi_probe_data->probe_data_list);
}

MultiProbeSensor::MultiProbeSensor()
{
  data_delete_func = multi_probe_data_destroy;
  class_name = "MultiProbeSensor";
}

MultiProbeSensor::~MultiProbeSensor()
{
}

bool MultiProbeSensor::init(void *args)
{
  multi_probe_sensor_init_options_t *opts = (multi_probe_sensor_init_options_t *)args;
  num_probe = opts->num_probe;
  push_button_pin = opts->push_button_pin;
  push_button_mode = opts->push_button_mode;
  if (push_button_mode == PUSH_BUTTON_MODE_SOFTWARE)
  {
    push_button_pin = -1;
  }
  else if (push_button_mode == PUSH_BUTTON_MODE_HARDWARE)
  {
    push_button_pin = opts->push_button_pin;
    pinMode(push_button_pin, INPUT);
  }
  else
  {
    push_button_pin = -1;
  }
  probe_list = new ProbeSensor[num_probe];
  ESP_LOGD(TAG, "Push Button Mode: %d", push_button_mode);
  ESP_LOGD(TAG, "Push Button Pin: %d", push_button_pin);
  ESP_LOGD(TAG, "Probes: %d", num_probe);
  for (int i = 0; i < num_probe; i++)
  {
    ESP_LOGD(TAG, "Probe pin: %d\n", opts->probe_sensor_opts_list[i]->probe_pin);
    probe_list[i].init((void *)(opts->probe_sensor_opts_list[i]));
  }
  if(push_button_mode == PUSH_BUTTON_MODE_ALWAYS_ON)
    push_button_value = 1;
  else
    push_button_value = 0;
  ESP_LOGD(TAG, "Multi Probe Sensor Initialized!\n");
}

void MultiProbeSensor::read(void *data)
{
  multi_probe_data_t *multi_probe_data = (multi_probe_data_t *)data;
  multi_probe_data->probe_data_list = (probe_data_t *)calloc(num_probe, sizeof(probe_data_t));
  for (int i = 0; i < num_probe; i++)
  {
    probe_list[i].read((void *)&multi_probe_data->probe_data_list[i]);
  }
  if (push_button_pin > 0 && push_button_mode == PUSH_BUTTON_MODE_HARDWARE)
  {
    push_button_value = multi_probe_data->push_button_state = digitalRead(push_button_pin); // to be changed
  }
  else if (push_button_mode == PUSH_BUTTON_MODE_SOFTWARE)
  {
    multi_probe_data->push_button_state = push_button_value;
  }
  else if (push_button_mode == PUSH_BUTTON_MODE_ALWAYS_ON)
  {
    multi_probe_data->push_button_state = 1;
  }
  else
  {
    multi_probe_data->push_button_state = -1;
  }
  multi_probe_data->count = num_probe;
  if ((push_button_mode == PUSH_BUTTON_MODE_SOFTWARE || push_button_mode == PUSH_BUTTON_MODE_ALWAYS_ON) && global_ws_client)
    global_ws_client->text(toCSV((void *)multi_probe_data));
  goodRead();
}

String MultiProbeSensor::toJson(void *data)
{
  multi_probe_data_t *multi_probe_data = (multi_probe_data_t *)data;
  dht_data_t *dht_data;
  String json_str = "{";
  for (int i = 0; i < num_probe; i++)
  {
    json_str += jsonStringVal("frequency" + String(i + 1)) + ":" + jsonStringVal(multi_probe_data->probe_data_list[i].frequency) + ",";
  }
  dht_data = (dht_data_t *)global_container.getLastSensorData("DHT1");
  json_str += jsonStringVal("temperature") + ":" + jsonStringVal(dht_data->temperature) + ",";
  json_str += jsonStringVal("humidity") + ":" + jsonStringVal(dht_data->humidity) + ",";
  json_str += jsonStringVal("state") + ":" + jsonStringVal(multi_probe_data->push_button_state);
  json_str += "}";
  return json_str;
}

String MultiProbeSensor::toCSV(void *data)
{
  multi_probe_data_t *multi_probe_data = (multi_probe_data_t *)data;
  dht_data_t *dht_data;
  String csv = "";
  int i;
  for (i = 0; i < multi_probe_data->count; i++)
  {
    csv += probe_list[0].toCSV((void *)&multi_probe_data->probe_data_list[i]) + ",";
  }
  dht_data = (dht_data_t *)global_container.getLastSensorData("DHT1");
  csv += jsonStringVal(dht_data->temperature) + ",";
  csv += jsonStringVal(dht_data->humidity) + ",";
  csv += String(multi_probe_data->push_button_state);
  return csv;
}

void *MultiProbeSensor::newData()
{
  multi_probe_data_t *tmp = NULL;

  tmp = (multi_probe_data_t *)calloc(1, sizeof(multi_probe_data_t));
  return tmp;
}

void MultiProbeSensor::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  int i = 0;
  *sensor_init_opts = calloc(1, sizeof(multi_probe_sensor_init_options_t));
  multi_probe_sensor_init_options_t *multi_probe_sensor_init_options = (multi_probe_sensor_init_options_t *)*sensor_init_opts;
  if (!multi_probe_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating Probe Sensor options!\n");
    return;
  }
  JsonArray probes = json_data["probes"].as<JsonArray>();
  multi_probe_sensor_init_options->num_probe = probes.size();
  multi_probe_sensor_init_options->probe_sensor_opts_list = (probe_sensor_init_options_t **)calloc(probes.size(), sizeof(probe_sensor_init_options_t *));
  ProbeSensor probe_sensor;
  for (JsonObject probe : probes)
  {
    probe_sensor.newSensorInitOptions((void **)&(multi_probe_sensor_init_options->probe_sensor_opts_list[i]), probe);
    i++;
  }
  multi_probe_sensor_init_options->push_button_pin = (uint8_t)json_data["push_button_pin"].as<unsigned short>();

  String button_mode = json_data["push_button_mode"].as<String>();
  button_mode.trim();
  button_mode.toUpperCase();
  if (button_mode == "SOFTWARE")
    multi_probe_sensor_init_options->push_button_mode = PUSH_BUTTON_MODE_SOFTWARE;
  else if (button_mode == "HARDWARE")
    multi_probe_sensor_init_options->push_button_mode = PUSH_BUTTON_MODE_HARDWARE;
  else
    multi_probe_sensor_init_options->push_button_mode = PUSH_BUTTON_MODE_ALWAYS_ON;
}

void MultiProbeSensor::printData(void *data)
{
  multi_probe_data_t *multi_probe_data = (multi_probe_data_t *)data;
  Serial.print("Multi Probe Frequency List : \n");
  for (int i = 0; i < num_probe; i++)
  {
    probe_list[i].printData((void *)&multi_probe_data->probe_data_list[i]);
  }
}

String MultiProbeSensor::process_control(const String &var)
{
  if (var == "SENSOR_NAME")
  {
    return String(getName());
  }
  else if (var == "EXP_BUTTON_VALUE")
  {
    return String(push_button_value);
  }
}

void MultiProbeSensor::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  if (push_button_mode == PUSH_BUTTON_MODE_SOFTWARE)
  {
    if (request->method() == HTTP_POST)
    {
      JsonObject body = json.as<JsonObject>();
      ESP_LOGD(TAG, "Changing button state. Original: %hu", push_button_value);
      if (body.containsKey("action"))
      {
        String button_id = body["action"].as<String>();
        if (button_id == "exp_state_toggle")
        {
          switch (push_button_value)
          {
          case 0:
            push_button_value = 1;
            break;
          default:
            push_button_value = 0;
          }
        }
        else if (button_id == "blow_state_toggle")
        {
          switch (push_button_value)
          {
          case 1:
            push_button_value = 2;
            break;
          case 2:
            push_button_value = 3;
            break;
          case 3:
            push_button_value = 2;
            break;
          default:
            break;
          }
        }
      }
      ESP_LOGD(TAG, "Changing button state. Changed: %hu", push_button_value);

      String json = "{";
      json += jsonStringVal("system_state") + ":" + jsonStringVal(push_button_value);
      json += "}";
      request->send(200, "application/json", json);
    }
    else
    {
      request->send(SPIFFS, "/multi_probe_sensor.html", String(), false, std::bind(&MultiProbeSensor::process_control, this, std::placeholders::_1));
    }
  }
  else if (push_button_mode == PUSH_BUTTON_MODE_ALWAYS_ON)
  {
    request->send(SPIFFS, "/multi_probe_sensor.html", String(), false, std::bind(&MultiProbeSensor::process_control, this, std::placeholders::_1));
  }
  else
  {
    request->send(200, "text/plain", "SOFTWARE mode DISABLED! Check configs!");
  }
}

bool MultiProbeSensor::registerApi()
{
  if (push_button_mode == PUSH_BUTTON_MODE_SOFTWARE)
    return true;
  if (push_button_mode == PUSH_BUTTON_MODE_ALWAYS_ON)
    return true;
  else
    return false;
}

bool MultiProbeSensor::isEnabled()
{
  if (push_button_mode == PUSH_BUTTON_MODE_SOFTWARE)
  {
    if (push_button_value > 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return true;
  }
}

void MultiProbeSensor::setLastData(void *new_data)
{
  if (new_data)
  {
    multi_probe_data_t *multi_probe_data = (multi_probe_data_t *)last_data;
    multi_probe_data_t *multi_probe_data_new = (multi_probe_data_t *)new_data;

    if (!multi_probe_data->probe_data_list)
      multi_probe_data->probe_data_list = (probe_data_t *)calloc(num_probe, sizeof(probe_data_t));
    SensorBase::setLastData(new_data);
    memcpy(multi_probe_data->probe_data_list, multi_probe_data_new->probe_data_list, num_probe * sizeof(probe_data_t));
  }
}

int MultiProbeSensor::getDataSize()
{
  return (sizeof(multi_probe_data_t));
}