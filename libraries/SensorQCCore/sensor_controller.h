#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "phase.h"
#include "sensor_base.h"
#include "qc_helpers.h"

typedef enum
{
    INIT = 0,
    PHASE,
    END,
    ABORTED
} mode;

typedef struct
{
    SensorBase *sensor;
    uint8_t num_phases;
    phase_opts_t *phase_opts;
    void *sensor_init_opts;
    sensor_basic_opts_t *sensor_basic_opts;
    JsonArray senders;
} sensor_controller_init_opts_t;

class SensorController
{
private:
    SensorBase *sensor;
    Phase **phase_list;
    uint8_t num_phases;
    uint8_t current_phase;
    mode current_mode;
    JsonArray senders;

public:
    SensorController(sensor_controller_init_opts_t sensor_controller_init_opts);
    ~SensorController();
    void updatePhaseList(int *durations);
    void loop();
    void resetAll(mode new_mode);
    mode getCurrentMode();
    String getSensorName();
    unsigned long getReadCount();
    unsigned long getFailCount();
    int getProgress();
    String getCurrentPhaseMessage();
    void **getPhaseList();
    mode getStatus();
    void stop();
    void handleApi(AsyncWebServerRequest *request, JsonVariant &json);
    bool registerSensorApi();
    void *getLastData();
    void setSensorID(unsigned int id);
};

#endif
