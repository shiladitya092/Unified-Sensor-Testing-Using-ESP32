#ifndef SENDER_H
#define SENDER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"

typedef struct sender_basic_opts_t
{
    String formatter;
} sender_basic_opts_t;

class Sender
{
protected:
    String formatter;

public:
    Sender();
    ~Sender();
    void senderBasicInit(sender_basic_opts_t &basic_opts);
    String toFormattedString(SensorBase *sensor, void **data, uint16_t count);
    virtual bool init(void *sender_options) = 0;
    virtual void newSenderInitOptions(void **sender_init_opts, JsonObject &json_data) = 0;
    virtual void loop() = 0;
    virtual bool sendPayload(uint8_t *payload, size_t length, JsonObject &options) = 0;
    virtual bool parseJsonOptions(JsonObject &options, void *sender_opts) = 0;
};

#endif