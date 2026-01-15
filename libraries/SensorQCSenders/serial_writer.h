#ifndef SERIAL_WRITER_H
#define SERIAL_WRITER_H

#include "sender.h"

typedef struct serial_opts_t
{
} serial_opts_t;

typedef struct serial_writer_opts_t
{
} serial_writer_opts_t;

class SerialWriter : public Sender
{
protected:
    bool serialWrite(uint8_t *payload, size_t length, serial_opts_t &serial_opts);

public:
    bool init(void *sender_options);
    void newSenderInitOptions(void **sender_init_opts, JsonObject &json_data);
    bool sendPayload(uint8_t *payload, size_t length, JsonObject &options);
    void loop();
    bool parseJsonOptions(JsonObject &options, void *sender_opts);
};

#endif
