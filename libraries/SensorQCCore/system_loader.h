#ifndef SYSTEM_LOADER_H
#define SYSTEM_LOADER_H

#include "drivers.h"
#include "configurator.h"
#include "phase.h"
#include "sensor_controller.h"
#include "controller_container.h"
#include "sensor_base.h"
#include "sender_container.h"

class SystemLoader
{
public:
    SensorBase *createSensor(String sensor_class);
    Sender *createSender(String sender_class);
    void initPhaseOptions(phase_opts_t *phase_opts, JsonObject &json_data);
    void initSensorBasicOptions(sensor_basic_opts_t *sensor_basic_opts, JsonObject &json_data);
    void initSensorInitOptions(SensorBase *sensor, void **sensor_init_opts, JsonObject &json_data);
    void initSenderBasicOptions(sender_basic_opts_t *sender_basic_opts, JsonObject &json_data);
    void initSenderOptions(Sender *sender, void **sender_init_opts, JsonObject &json_data);
    SensorController *createSensorController(SensorBase *sensor, phase_opts_t *phase_opts, void *sensor_init_opts, sensor_basic_opts_t *sensor_basic_opts, JsonArray senders);
    void createControllerContainer();
    void createSenderContainer();
    void setupMdhDiscoveryClient();
};

extern SystemLoader system_loader;

#endif