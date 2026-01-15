#include <Arduino.h>
#include "sender.h"
#include "serial_writer.h"

bool SerialWriter::init(void *sender_options)
{
    serial_writer_opts_t *serial_writer_opts = (serial_writer_opts_t *)sender_options;

    return true;
}

bool SerialWriter::serialWrite(uint8_t *payload, size_t length, serial_opts_t &serial_opts)
{
    if (payload[length - 1] != 0)
    {
        char tmp[length + 1];
        memcpy(tmp, payload, length);
        tmp[length] = 0;
        Serial.printf("%s", (char *)tmp);
    }
    else
    {
        Serial.printf("%s", (char *)payload);
    }
    Serial.print("\n");
}

void SerialWriter::loop()
{
}

void SerialWriter::newSenderInitOptions(void **sender_init_opts, JsonObject &json_data)
{
    *sender_init_opts = calloc(1, sizeof(serial_writer_opts_t));
    serial_writer_opts_t *serial_writer_opts = (serial_writer_opts_t *)*sender_init_opts;
    if (!serial_writer_opts)
    {
        Serial.println("Error creating SerialWriter options!");
        return;
    }
}

bool SerialWriter::sendPayload(uint8_t *payload, size_t length, JsonObject &options)
{
    serial_opts_t serial_opts;

    parseJsonOptions(options, (void *)&serial_opts);
    return serialWrite(payload, length, serial_opts);
}

bool SerialWriter::parseJsonOptions(JsonObject &options, void *sender_opts)
{
    serial_opts_t *serial_opts = (serial_opts_t *)sender_opts;
    return true;
}