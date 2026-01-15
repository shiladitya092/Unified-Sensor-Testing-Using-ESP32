#include <Arduino.h>
#include <ArduinoJson.h>
#include "sensor_base.h"
#include "sender.h"


Sender::Sender()
{
}

Sender::~Sender()
{
}

void Sender::senderBasicInit(sender_basic_opts_t &basic_opts)
{
    formatter = String(basic_opts.formatter);
    ESP_LOGD(TAG,"Using formatter function: %s\n", formatter.c_str());
}

String Sender::toFormattedString(SensorBase *sensor, void **data, uint16_t count)
{
    if (formatter == "toJson")
        return sensor->toJson(data, count);
    else if (formatter == "toCSV")
        return sensor->toCSV(data, count);
    else if (formatter == "asRAW")
        return sensor->asRAW(data, count);
    else
    {
        ESP_LOGD(TAG,"Invalid formatter: %s\n", formatter.c_str());
        return String("");
    }
}