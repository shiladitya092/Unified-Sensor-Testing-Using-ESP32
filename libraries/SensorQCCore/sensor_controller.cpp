#include <Arduino.h>
#include <ArduinoJson.h>
#include "phase.h"
#include "sensor_base.h"
#include "qc_helpers.h"
#include "sender_container.h"
#include "sensor_controller.h"
#include "configurator.h"
#include "time_operations.h"

SensorController::SensorController(sensor_controller_init_opts_t sensor_controller_init_opts)
{
    uint8_t i;
    sensor = sensor_controller_init_opts.sensor;
    num_phases = sensor_controller_init_opts.num_phases;
    current_phase = 0;
    phase_list = (Phase **)calloc(num_phases, sizeof(Phase *));
    senders = sensor_controller_init_opts.senders;
    for (i = 0; i < num_phases; i++)
    {
        phase_list[i] = new Phase();
        phase_list[i]->init(sensor_controller_init_opts.phase_opts[i]);
        phase_list[i]->update(sensor_controller_init_opts.phase_opts[i].duration);
    }
    current_mode = INIT;
    sensor->init(sensor_controller_init_opts.sensor_init_opts);
    sensor->sensorBasicInit(sensor_controller_init_opts.sensor_basic_opts);
    sensor->initLastData();
}

SensorController::~SensorController()
{
    free(phase_list);
    phase_list = NULL;
}

void SensorController::updatePhaseList(int *durations)
{
    uint8_t i;

    for (i = 0; i < num_phases; i++)
    {
        phase_list[i]->update(durations[i]);
    }
}

void SensorController::loop()
{
    if (phase_list[current_phase]->isCheckpoint(sensor->getLastReadTime()))
    {
        ESP_LOGD(TAG, "Reading from Sensor: %s\n", sensor->getName().c_str());
        if (sensor->isEnabled())
        {
            sensor_data_t *sensor_data = (sensor_data_t *)calloc(1, sizeof(sensor_data_t));
            sensor_data->data = sensor->newData();
            if (sensor_data->data)
            {
                sensor_data->data_delete_func = sensor->data_delete_func;
                sensor->read(sensor_data->data);
                sensor->setLastData(sensor_data->data);
                sensor_data->timestamp = get_soft_time_ms();
                phase_list[current_phase]->add(sensor_data);
                // sensor->printData(sensor_data->data);
                ESP_LOGD(TAG, "Read from Sensor: %s successful!\n", sensor->getName().c_str());
            }
            else
            {
                free(sensor_data);
                sensor->dummyRead();
                ESP_LOGD(TAG, "Read from Sensor: %s failed as data was not created!\n", sensor->getName().c_str());
            }
        }
        else
        {
            sensor->dummyRead();
            ESP_LOGD(TAG, "Read from Sensor: %s aborted as sensor is in disabled state!\n", sensor->getName().c_str());
        }
        if (phase_list[current_phase]->isComplete())
        {
            for (JsonObject sender : senders)
            {
                String sender_name = sender["name"].as<String>();
                JsonObject options = sender["sender_options"].as<JsonObject>();
                void **data_array;
                uint16_t count = 0;
                count = phase_list[current_phase]->getN(phase_list[current_phase]->getCount(), &data_array);
                if (global_sender_container.send(sender_name, sensor, data_array, count, options))
                    ESP_LOGD(TAG, "Packet sent by %s successfully.\n", sender_name.c_str());
                else
                    ESP_LOGD(TAG, "Failed to send packet sent by %s!\n", sender_name.c_str());
            }
            phase_list[current_phase]->delete_data(sensor_data_destroy);
            phase_list[current_phase]->reset();
        }
    }
}

void SensorController::resetAll(mode new_mode)
{
    current_mode = new_mode;
    current_phase = 0;
    sensor->resetReadCount();
}

mode SensorController::getCurrentMode()
{
    return current_mode;
}

String SensorController::getSensorName()
{
    return sensor->getName();
}

unsigned long SensorController::getReadCount()
{
    return sensor->getReadCount();
}

unsigned long SensorController::getFailCount()
{
    return sensor->getFailCount();
}

int SensorController::getProgress()
{
    // ESP_LOGD(TAG,"Current Phase: %d\n", current_phase));
    // ESP_LOGD(TAG,"Total phases: %d\n", num_phases);
    if (current_mode == END)
        return 100;
    return (((float)current_phase / num_phases) * 100);
}

String SensorController::getCurrentPhaseMessage()
{
    return phase_list[current_phase]->getMessage();
}

void **SensorController::getPhaseList()
{
    return (void **)phase_list;
}

mode SensorController::getStatus()
{
    return current_mode;
}

void SensorController::stop()
{
    current_mode = ABORTED;
}

void SensorController::handleApi(AsyncWebServerRequest *request, JsonVariant &json)
{
    ESP_LOGD(TAG, "Sensor %s is handling the request...", sensor->getName().c_str());
    sensor->handleApi(request, json);
}

bool SensorController::registerSensorApi()
{
    return sensor->registerApi();
}

void *SensorController::getLastData()
{
    return sensor->getLastData();
}

void SensorController::setSensorID(unsigned int id)
{
    sensor->setID(id);
}