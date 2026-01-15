#include <Arduino.h>
#include <ArduinoJson.h>
#include "moisture_sensor.h"
#include "time_operations.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void moisture_data_destroy(void *data)
{
}

MoistureSensor::MoistureSensor()
{
  data_delete_func = moisture_data_destroy;
  class_name = "MoistureSensor";
}

MoistureSensor::~MoistureSensor()
{
}

bool MoistureSensor::init(void *args)
{
  moisture_sensor_init_options_t *opts = (moisture_sensor_init_options_t *)args;
  ESP_LOGD(TAG, "Initializing Moisture Sensor at Pin X: %d & Pin Y: %d\n", opts->analog_pin_x, opts->analog_pin_y);
  this->pin_x = opts->analog_pin_x;
  this->pin_y = opts->analog_pin_y;
  ESP_LOGD(TAG, "Moisture Sensor Initialized!\n");
}

void MoistureSensor::read(void *data)
{
  moisture_data_t *moisture_data = (moisture_data_t *)data;
  int num_readings = 10;

  readStableADC(num_readings, pin_x, moisture_data->value_mv_x, moisture_data->std_dev_x);
  readStableADC(num_readings, pin_y, moisture_data->value_mv_y, moisture_data->std_dev_y);
  performCalculations(moisture_data);
  goodRead();
}

// Calculation related functions should be put into a seperate file
bool MoistureSensor::performCalculations(moisture_data_t *moisture_data)
{
  moisture_data->magnitude = sqrt(sq(moisture_data->value_mv_x) + sq(moisture_data->value_mv_y));
  moisture_data->phase_out = atan2(moisture_data->value_mv_y, moisture_data->value_mv_x) * (180 / 3.14);
  return true;
}

bool MoistureSensor::readADC(float &value_adc, int channel)
{
#ifdef TEST_MODE
  value_adc = (float)random(0, 1024);
#else
  value_adc = analogRead(channel);
#endif
}

bool MoistureSensor::readStableADC(int num_readings, int channel, float &value_mv, float &std_dev)
{
  float adc_value_array[num_readings];
  float value_adc_tmp;
  int i;

  for (i = 0; i < num_readings; i++)
  {
    readADC(value_adc_tmp, channel);
    // Serial.println("value_adc_tmp:" + String(value_adc_tmp) + ".......................");
    adc_value_array[i] = (map(value_adc_tmp, 0, 4095, 0, 3300) + 100) / 1000.0; // +100 is used for temporary calibration
  }
  value_mv = stats.median(adc_value_array, num_readings);
  std_dev = stats.stdev(adc_value_array, num_readings);
  return true;
}

String MoistureSensor::toJson(void *data)
{
  moisture_data_t *moisture_data = (moisture_data_t *)data;
  String json_str = "{";
  json_str += jsonStringVal("value_mv_x") + ":" + jsonStringVal((unsigned int)moisture_data->value_mv_x);
  json_str += ",";
  json_str += jsonStringVal("value_mv_y") + ":" + jsonStringVal((unsigned int)moisture_data->value_mv_y);
  json_str += ",";
  json_str += jsonStringVal("std_dev_x") + ":" + jsonStringVal((unsigned int)moisture_data->std_dev_x);
  json_str += ",";
  json_str += jsonStringVal("std_dev_y") + ":" + jsonStringVal((unsigned int)moisture_data->std_dev_y);
  json_str += ",";
  json_str += jsonStringVal("magnitude") + ":" + jsonStringVal((unsigned int)moisture_data->magnitude);
  json_str += ",";
  json_str += jsonStringVal("phase_out") + ":" + jsonStringVal((unsigned int)moisture_data->phase_out);
  json_str += "}";
  return json_str;
}

String MoistureSensor::toCSV(void *data)
{
  moisture_data_t *moisture_data = (moisture_data_t *)data;
  String csv = "";

  csv += String(moisture_data->value_mv_x) + ",";
  csv += String(moisture_data->value_mv_y) + ",";
  csv += String(moisture_data->std_dev_x) + ",";
  csv += String(moisture_data->std_dev_y) + ",";
  csv += String(moisture_data->magnitude) + ",";
  csv += String(moisture_data->phase_out);
  return csv;
}

void *MoistureSensor::newData()
{
  moisture_data_t *tmp = (moisture_data_t *)calloc(1, sizeof(moisture_data_t));
  return tmp;
}

void MoistureSensor::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(moisture_sensor_init_options_t));
  moisture_sensor_init_options_t *moisture_sensor_init_options = (moisture_sensor_init_options_t *)*sensor_init_opts;
  if (!moisture_sensor_init_options)
  {
    ESP_LOGD(TAG, "Error creating Moisture Sensor options!\n");
    return;
  }
  moisture_sensor_init_options->analog_pin_x = (uint8_t)json_data["pin1"].as<unsigned short>();
  moisture_sensor_init_options->analog_pin_y = (uint8_t)json_data["pin2"].as<unsigned short>();
}

void MoistureSensor::printData(void *data)
{
  moisture_data_t *moisture_data = (moisture_data_t *)data;

  Serial.print("Value_mv_X: ");
  Serial.print(String(moisture_data->value_mv_x));
  Serial.print(", Value_mv_Y: ");
  Serial.print(String(moisture_data->value_mv_y));
  Serial.print(", std_dev_x: ");
  Serial.print(String(moisture_data->std_dev_x));
  Serial.print(", std_dev_y: ");
  Serial.print(String(moisture_data->std_dev_y));
  Serial.print(", Magnitude: ");
  Serial.print(String(moisture_data->magnitude));
  Serial.print(", Phase Out: ");
  Serial.print(String(moisture_data->phase_out));
}

void MoistureSensor::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool MoistureSensor::registerApi()
{
  return false;
}

bool MoistureSensor::isEnabled()
{
  return true;
}

int MoistureSensor::getDataSize()
{
  return sizeof(moisture_data_t);
}