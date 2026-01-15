#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include "ADS7844.h"
#include "qc_helpers.h"
#include "sensor_base.h"

void ads7844_data_destroy(void *data)
{
}

ADS7844::ADS7844()
{
  data_delete_func = ads7844_data_destroy;
}

ADS7844::~ADS7844()
{
}

bool ADS7844::init(void *args)
{
  ads7844_init_options_t *opts = (ads7844_init_options_t *)args;
  ESP_LOGD(TAG, "Initializing ADS7844 Sensor with CS Pin: %d \n", opts->cs_pin);
  cs_pin = opts->cs_pin;
  iterations = opts->iterations;
  pinMode(cs_pin, OUTPUT);
  digitalWrite(cs_pin, HIGH);
  SPI.begin();
  ESP_LOGD(TAG, "ADS7844 Sensor Initialized!\n");
}

float ADS7844::read_adc()
{
  byte commandByte;
  byte result[2];
  int resultInt = 0;

  for (byte x = 2; x < 3; x++)
  {
    // set to adc# and bit3 HIGH
    commandByte = x + 8;
    // shift into position
    commandByte = commandByte << 4;
    // set low 4 bits HIGH
    commandByte = commandByte | 0x0f;

    // enable ADC SPI slave select
    digitalWrite(cs_pin, LOW);
    delayMicroseconds(2);

    SPI.transfer(commandByte);
    // you might need to increase this delay for conversion
    delayMicroseconds(4);

    // get results
    result[0] = SPI.transfer(0);
    result[1] = SPI.transfer(0);

    // disable ADC SPI slave select
    digitalWrite(cs_pin, HIGH);

    resultInt = result[0];
    resultInt = resultInt << 8;
    resultInt = resultInt | result[1];
    resultInt = resultInt >> 3;
    resultInt = resultInt * 1.0;

    return (float)((resultInt * 3300) / 4095.0);
  }
}

void ADS7844::calculate_meta(float *data, int count, ads7844_data_t *output)
{
  int i;

  double sum = 0.0;
  for (i = 0; i < count; i++)
  {
    sum += (double)data[i];
  }
  double avg = sum / count;

  double sd_sum = 0.0;
  for (int i = 0; i < count; i++)
  {
    sd_sum += ((double)data[i] - avg) * ((double)data[i] - avg);
  }
  double sd = (double)sd_sum / count;
  sd = sqrt(sd);

  int j = 0;
  double clean[count];
  for (i = 0; i < count; i++)
  {
    if (abs(data[i] - avg) <= 2 * sd)
    {
      clean[j] = data[i];
      j++;
    }
  }

  double clean_sum = 0;
  for (i = 0; i < j; i++)
  {
    clean_sum += clean[i];
  }
  double cleanAvg = clean_sum / j;

  ESP_LOGD(TAG, "%lf,%lf,%lf,%lf", avg, sd, cleanAvg, j);
  output->average = avg;
  output->std_dev = sd;
  output->clean_avg = cleanAvg;
  output->clean_count = j;
}

void ADS7844::read(void *data)
{
  ads7844_data_t *ads7844_data = (ads7844_data_t *)data;
  int i;

  float raw_data[iterations];
  goodRead(); // Kludgie: This should be done after the read is complete. But since the read itself takes over 1s, this is done to remove the interval between two cycles.
  for (i = 0; i < iterations; i++)
  {
    raw_data[i] = read_adc();
    delay(20);
  }
  calculate_meta(raw_data, iterations, ads7844_data);
}

String ADS7844::toJson(void *data)
{
  ads7844_data_t *ads7844_data = (ads7844_data_t *)data;
  String json_str = "{";
  json_str += jsonStringVal("average") + ":" + jsonStringVal(ads7844_data->average) + ",";
  json_str += jsonStringVal("std_dev") + ":" + jsonStringVal(ads7844_data->std_dev) + ",";
  json_str += jsonStringVal("clean_avg") + ":" + jsonStringVal(ads7844_data->clean_avg) + ",";
  json_str += jsonStringVal("clean_count") + ":" + jsonStringVal(ads7844_data->clean_count);
  json_str += "}";
  return json_str;
}

String ADS7844::toCSV(void *data)
{
  ads7844_data_t *ads7844_data = (ads7844_data_t *)data;
  String csv = "";

  csv += String(ads7844_data->average) + ",";
  csv += String(ads7844_data->std_dev) + ",";
  csv += String(ads7844_data->clean_avg) + ",";
  csv += String(ads7844_data->clean_count);
  return csv;
}

void *ADS7844::newData()
{
  ads7844_data_t *tmp = (ads7844_data_t *)calloc(1, sizeof(ads7844_data_t));
  return tmp;
}

void ADS7844::newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data)
{
  *sensor_init_opts = calloc(1, sizeof(ads7844_init_options_t));
  ads7844_init_options_t *ads7844_init_options = (ads7844_init_options_t *)*sensor_init_opts;
  if (!ads7844_init_options)
  {
    ESP_LOGD(TAG, "Error creating ADS7844 Sensor options!\n");
    return;
  }
  ads7844_init_options->cs_pin = (uint8_t)json_data["cs_pin"].as<unsigned short>();
  ads7844_init_options->iterations = (uint8_t)json_data["iterations"].as<int>();
}

void ADS7844::printData(void *data)
{
  ads7844_data_t *ads7844_data = (ads7844_data_t *)data;
}

void ADS7844::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
  request->send(200, "text/plain", "OK");
}

bool ADS7844::registerApi()
{
  return false;
}

bool ADS7844::isEnabled()
{
  return true;
}

int ADS7844::getDataSize()
{
  return sizeof(ads7844_data_t);
}