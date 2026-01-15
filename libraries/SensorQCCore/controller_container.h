#ifndef CONTROLLER_CONTAINER_H
#define CONTROLLER_CONTAINER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "sensor_controller.h"

typedef struct sensor_api_list_t
{
    int count;
    String *routes;
} sensor_api_list_t;

class ControllerContainer
{
private:
    SensorController **container;
    uint8_t container_size;
    uint8_t controller_count;
    int current_sensor;
    int data_type_size;

public:
    ControllerContainer();
    ~ControllerContainer();
    bool init(uint8_t size);
    bool addSensorController(SensorController *controller);
    void loop();
    void handleApi(String &sensor_name, AsyncWebServerRequest *request, JsonVariant &json);
    void listSensorApi(sensor_api_list_t &sensor_api_list);
    void *getLastSensorData(String sensor_name);
};

extern ControllerContainer global_container;

#endif
