#ifndef SENSOR_BASE_H
#define SENSOR_BASE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

typedef void (*sensor_data_delete)(void *);

typedef struct sensor_basic_opts_t
{
    String name;
} sensor_basic_opts_t;

typedef struct sensor_data_t
{
    void *data;
    unsigned long long timestamp;
    sensor_data_delete data_delete_func;
} sensor_data_t;

void sensor_data_destroy(void *data_ptr);

class SensorBase
{
private:
    String name;
    unsigned int id;
    unsigned int read_count;
    unsigned int fail_count;
    unsigned long last_read_time; // in milliseconds
    virtual String toCSV(void *data) { return String(""); }
    virtual String toJson(void *data) { return String(""); }
    virtual String asRAW(void *data) { return String(""); }

protected:
    String class_name;
    void *last_data = NULL;

public:
    SensorBase();
    ~SensorBase();
    virtual bool init(void *args) = 0;
    virtual void read(void *) = 0;
    String toJson(void **data, uint16_t count);
    String toCSV(void **data, uint16_t count);
    String asRAW(void **data, uint16_t count);
    virtual void *newData() = 0;
    virtual int getDataSize() { return 0; }
    virtual void newSensorInitOptions(void **sensor_init_opts, JsonObject &json_data) = 0;
    virtual void handleApi(AsyncWebServerRequest *request, JsonVariant &json) = 0;
    virtual void printData(void *data) = 0;
    virtual bool registerApi() = 0;
    virtual bool isEnabled() = 0;
    void sensorBasicInit(sensor_basic_opts_t *sensor_basic_opts);
    void goodRead();
    void dummyRead();
    void failedRead();
    unsigned long getFailCount();
    unsigned long getReadCount();
    String getName();
    unsigned int getID();
    void setID(unsigned int id);
    unsigned long getLastReadTime();
    void resetReadCount();
    void initLastData();
    void setLastData(void *new_data);
    void *getLastData();

    sensor_data_delete data_delete_func;
};

#endif