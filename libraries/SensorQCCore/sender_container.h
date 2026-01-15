#ifndef SENDER_CONTAINER_H
#define SENDER_CONTAINER_H

#include "sender.h"


typedef struct sender_t
{
    String name;
    Sender *sender;
} sender_t;

class SenderContainer
{
private:
    sender_t *sender_array;
    int max_senders;
    int num_senders;

public:
    void init(int num);
    bool addSender(Sender *sender, String name);
    bool send(String sender_name, SensorBase *sensor, void **data, uint16_t count, JsonObject &options);
    void loop();
};

extern SenderContainer global_sender_container;

#endif