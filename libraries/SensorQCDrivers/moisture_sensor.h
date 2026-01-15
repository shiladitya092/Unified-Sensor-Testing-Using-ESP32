#ifndef INC_FREQUENCY_CAPTURE_H_
#define INC_FREQUENCY_CAPTURE_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"
#include <QuickStats.h>

typedef struct moisture_sensor_init_options_t
{
    uint8_t analog_pin_x;
    uint8_t analog_pin_y;

} moisture_sensor_init_options_t;

typedef struct moisture_data_t
{
    float value_mv_x;
    float value_mv_y;
    float std_dev_x;
    float std_dev_y;
    float magnitude;
    float phase_out;
} moisture_data_t;

void moisture_data_destroy(void *data);

class MoistureSensor : public SensorBase
{
private:
    QuickStats stats;
    uint8_t pin_x;
    uint8_t pin_y;

    bool readADC(float &value_adc, int channel);
    bool readStableADC(int num_readings, int channel, float &value_mv, float &std_dev);
    bool performCalculations(moisture_data_t *moisture_data);

public:
    MoistureSensor();
    ~MoistureSensor();
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

#endif /* INC_MOISTURE_SENSOR_H_ */
