#include <ArduinoJson.h>
#include <Arduino.h>
#include "system_loader.h"
#include "drivers.h"
#include "configurator.h"
#include "phase.h"
#include "sensor_controller.h"
#include "controller_container.h"
#include "sensor_base.h"
#include "sender_container.h"
#include "senders.h"
#include "mdh_discovery_client.h"

SystemLoader system_loader;

SensorBase *SystemLoader::createSensor(String sensor_class)
{
    SensorBase *sensor = (SensorBase *)createSensorObject(sensor_class);
    if (!sensor)
        ESP_LOGD(TAG,"Unable to create sensor of the type: %s\n", sensor_class.c_str());
    return sensor;
}

Sender *SystemLoader::createSender(String sender_class)
{
    Sender *sender = (Sender *)createSenderObject(sender_class);
    if (!sender)
        ESP_LOGD(TAG,"Unable to create sender of the type: %s\n", sender_class.c_str());
    return sender;
}

void SystemLoader::initPhaseOptions(phase_opts_t *phase_opts, JsonObject &json_data)
{
    phase_opts->gather = true;
    phase_opts->autonext = true;
    phase_opts->duration = json_data["duration"].as<float>();;
    phase_opts->message = "Default Phase";
    phase_opts->sampling_rate = json_data["sampling_rate"].as<float>();
}

void SystemLoader::initSensorBasicOptions(sensor_basic_opts_t *sensor_basic_opts, JsonObject &json_data)
{
    sensor_basic_opts->name = json_data["name"].as<String>();
}

void SystemLoader::initSensorInitOptions(SensorBase *sensor, void **sensor_init_opts, JsonObject &json_data)
{
    sensor->newSensorInitOptions(sensor_init_opts, json_data);
}

void SystemLoader::initSenderBasicOptions(sender_basic_opts_t *sender_basic_opts, JsonObject &json_data)
{
    sender_basic_opts->formatter = json_data["formatter"].as<String>();
}

void SystemLoader::initSenderOptions(Sender *sender, void **sender_init_opts, JsonObject &json_data)
{
    sender->newSenderInitOptions(sender_init_opts, json_data);
}

SensorController *SystemLoader::createSensorController(SensorBase *sensor, phase_opts_t *phase_opts, void *sensor_init_opts, sensor_basic_opts_t *sensor_basic_opts, JsonArray senders)
{
    sensor_controller_init_opts_t controller_opts;
    controller_opts.num_phases = 1;
    controller_opts.sensor = sensor;
    controller_opts.phase_opts = phase_opts;
    controller_opts.sensor_init_opts = sensor_init_opts;
    controller_opts.sensor_basic_opts = sensor_basic_opts;
    controller_opts.senders = senders;
    SensorController *controller = new SensorController(controller_opts);
    return controller;
}

void SystemLoader::createControllerContainer()
{
    JsonArray sensors = global_configurator.config_json["sensors"].as<JsonArray>();
    global_container.init(sensors.size());
    for (JsonObject sensor : sensors)
    {
        String type = sensor["type"].as<String>();
        ESP_LOGD(TAG,"Found sensor of type: %s, creating...\n", type.c_str());
        SensorBase *sensor_ptr = createSensor(type);
        if (sensor_ptr)
        {
            ESP_LOGD(TAG,"Sensor of type: %s created! Initializing...\n", type.c_str());
            phase_opts_t phase_opts;
            sensor_basic_opts_t sensor_basic_opts;
            void *sensor_init_opts;
            SensorController *controller;
            JsonObject phase_opts_json = sensor["phase_options"].as<JsonObject>();
            JsonObject sensor_basic_opts_json = sensor["sensor_basic_options"].as<JsonObject>();
            JsonObject sensor_init_opts_json = sensor["sensor_init_options"].as<JsonObject>();
            JsonArray senders = sensor["senders"].as<JsonArray>();
            initPhaseOptions(&phase_opts, phase_opts_json);
            initSensorBasicOptions(&sensor_basic_opts, sensor_basic_opts_json);
            initSensorInitOptions(sensor_ptr, &sensor_init_opts, sensor_init_opts_json);
            controller = createSensorController(sensor_ptr, &phase_opts, sensor_init_opts, &sensor_basic_opts, senders);
            if (controller)
            {
                ESP_LOGD(TAG,"Controller for sensor of type: %s created! Adding...\n", type.c_str());
                global_container.addSensorController(controller);
            }
            free(sensor_init_opts);
        }
    }
}

void SystemLoader::createSenderContainer()
{
    JsonArray senders = global_configurator.config_json["senders"].as<JsonArray>();
    global_sender_container.init(senders.size());
    for (JsonObject sender : senders)
    {
        String name = sender["name"].as<String>();
        String type = sender["type"].as<String>();
        ESP_LOGD(TAG,"Found sender of type: %s, creating...\n", type.c_str());
        Sender *sender_ptr = createSender(type);
        sender_basic_opts_t sender_basic_opts;
        void *sender_init_opts;
        JsonObject sender_basic_opts_json = sender["sender_basic_options"].as<JsonObject>();
        JsonObject sender_init_opts_json = sender["sender_options"].as<JsonObject>();
        initSenderBasicOptions(&sender_basic_opts, sender_basic_opts_json);
        initSenderOptions(sender_ptr, &sender_init_opts, sender_init_opts_json);
        sender_ptr->senderBasicInit(sender_basic_opts);
        sender_ptr->init(sender_init_opts);
        global_sender_container.addSender(sender_ptr, name);
        free(sender_init_opts);
    }
}

void SystemLoader::setupMdhDiscoveryClient()
{
    JsonObject options_json = global_configurator.config_json["mdh_discovery_client"].as<JsonObject>();
    if (options_json["enabled"].as<bool>())
    {
        mdh_discovery_init_opts_t mdh_discovery_init_opts;
        mdh_discovery_init_opts.server_name = options_json["server_name"].as<String>();
        mdh_discovery_init_opts.client_name = options_json["client_name"].as<String>();
        mdh_discovery_init_opts.server_port = options_json["server_port"].as<unsigned int>();
        mdh_discovery_init_opts.client_port = options_json["client_port"].as<unsigned int>();
        mdh_discovery_init_opts.targets = options_json["targets"].as<JsonArray>();
        mdh_discovery_init_opts.default_server = options_json["default_server"].as<String>();
        mdh_discovery_client.init(mdh_discovery_init_opts);
    }
}