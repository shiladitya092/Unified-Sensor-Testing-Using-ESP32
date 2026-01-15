#include <Arduino.h>
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "controller_container.h"
#include "sensor_controller.h"

ControllerContainer global_container;

ControllerContainer::ControllerContainer()
{
    container = NULL;
    current_sensor = 0;
    container_size = 0;
    controller_count = 0;
}

ControllerContainer::~ControllerContainer()
{
    if (container)
        free(container);
    container = NULL;
}

bool ControllerContainer::init(uint8_t size)
{
    uint8_t i;
    if (size < 1)
        return false;
    container_size = size;
    container = (SensorController **)calloc(container_size, sizeof(SensorController *));
    if (!container)
        return false;
    return true;
}

bool ControllerContainer::addSensorController(SensorController *controller)
{
    if (controller_count == container_size)
        return false;
    container[controller_count] = controller;
    container[controller_count]->setSensorID(controller_count);
    controller_count++;
    return true;
}

void ControllerContainer::loop()
{
    if (container_size > 0)
    {
        container[current_sensor++]->loop();
        if (current_sensor == controller_count)
            current_sensor = 0;
    }
}

void ControllerContainer::handleApi(String &sensor_name, AsyncWebServerRequest *request, JsonVariant &json)
{
    int i;
    for (i = 0; i < container_size; i++)
    {
        if (container[i]->registerSensorApi() && container[i]->getSensorName() == sensor_name)
        {
            container[i]->handleApi(request, json);
            return;
        }
    }
    request->send(200, "text/plain", "The requested sensor was not found in the system!");
}

void ControllerContainer::listSensorApi(sensor_api_list_t &sensor_api_list)
{
    int i, count = 0;
    for (i = 0; i < container_size; i++)
    {
        if (container[i]->registerSensorApi())
            count++;
    }
    sensor_api_list.count = count;
    sensor_api_list.routes = new String[count];
    count = 0;
    for (i = 0; i < container_size; i++)
    {
        if (container[i]->registerSensorApi())
        {
            sensor_api_list.routes[count] = container[i]->getSensorName();
            count++;
        }
    }
}

void *ControllerContainer::getLastSensorData(String sensor_name)
{
    int i;
    
    for (i = 0; i < container_size; i++)
    {
        if (sensor_name == container[i]->getSensorName())
        {
            return container[i]->getLastData();
        }
    }
    return NULL;
}
