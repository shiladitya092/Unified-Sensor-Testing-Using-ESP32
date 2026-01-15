#include <Arduino.h>
#include "sensor_base.h"
#include "qc_helpers.h"

void sensor_data_destroy(void *data_ptr)
{
    sensor_data_t *sensor_data = (sensor_data_t *)data_ptr;
    if (sensor_data->data)
    {
        sensor_data->data_delete_func(sensor_data->data);
        free(sensor_data->data);
    }
    free(data_ptr);
}

SensorBase::SensorBase()
{
    read_count = 0;
    fail_count = 0;
    class_name = "SensorBase";
}

SensorBase::~SensorBase()
{
    free(last_data);
}

void SensorBase::goodRead()
{
    read_count++;
    last_read_time = millis();
}

void SensorBase::dummyRead()
{
    last_read_time = millis();
}
void SensorBase::failedRead()
{
    goodRead();
    fail_count++;
}

unsigned long SensorBase::getFailCount()
{
    return fail_count;
}

unsigned long SensorBase::getReadCount()
{
    return read_count;
}

String SensorBase::getName()
{
    return name;
}

unsigned int SensorBase::getID()
{
    return id;
}

void SensorBase::setID(unsigned int id)
{
    this->id = id;
}

unsigned long SensorBase::getLastReadTime()
{
    return last_read_time;
}

void SensorBase::sensorBasicInit(sensor_basic_opts_t *sensor_basic_opts)
{
    name = String(sensor_basic_opts->name);
}

void SensorBase::resetReadCount()
{
    read_count = 0;
    fail_count = 0;
}

String SensorBase::toJson(void **data, uint16_t count)
{
    int i;
    String json = "";
    sensor_data_t *sensor_data;

    json += "{";
    json += jsonStringVal("macid") + ":" + jsonStringVal(WiFi.macAddress()) + ",";
    json += jsonStringVal("sensors") + ":";
    json += "[{";
    json += jsonStringVal("sensor_type") + ":" + jsonStringVal(class_name) + ",";
    json += jsonStringVal("sensor_id") + ":" + jsonStringVal(this->getID()) + ",";
    json += jsonStringVal("data") + ":";
    json += "[";
    for (i = 0; i < count; i++)
    {
        sensor_data = (sensor_data_t *)data[i];
        json += "{";
        json += jsonStringVal("items") + ":" + toJson(sensor_data->data) + ",";
        json += jsonStringVal("timestamp") + ":" + jsonStringVal(sensor_data->timestamp);
        json += "}";
        if (i < count - 1)
            json += ",";
    }
    json += "]}]}";
    return json;
}

String SensorBase::toCSV(void **data, uint16_t count)
{
    int i;
    String csv = "";
    sensor_data_t *sensor_data;
    for (i = 0; i < count; i++)
    {
        sensor_data = (sensor_data_t *)data[i];
        csv += jsonStringVal(sensor_data->timestamp) + ","; // Make another func for CSV conversions??
        csv += this->toCSV(sensor_data->data);
        if (i < count - 1)
            csv += "\n";
    }
    return csv;
}

String SensorBase::asRAW(void **data, uint16_t count)
{
    int i;
    String raw = "";
    sensor_data_t *sensor_data;
    for (i = 0; i < count; i++)
    {
        sensor_data = (sensor_data_t *)data[i];
        raw = this->asRAW(sensor_data->data);       // Kludgie here with raw 
    }
    return raw;
}

void SensorBase::initLastData()
{
    last_data = calloc(1, this->getDataSize());
}

void SensorBase::setLastData(void *new_data)
{
    if(new_data)
        memcpy(last_data, new_data, this->getDataSize());
}

void *SensorBase::getLastData()
{
    return last_data;
}