#include "sender_container.h"
#include "sender.h"


SenderContainer global_sender_container;

void SenderContainer::init(int num)
{
    num_senders = 0;
    max_senders = num;
    sender_array = (sender_t *)calloc(num, sizeof(sender_t));
}

bool SenderContainer::addSender(Sender *sender, String name)
{
    if (num_senders < max_senders)
    {
        sender_array[num_senders].sender = sender;
        sender_array[num_senders++].name = String(name);
        return true;
    }
    else
    {
        ESP_LOGD(TAG,"Could not add sender, limit reached!\n");
        return false;
    }
}

bool SenderContainer::send(String sender_name, SensorBase *sensor, void **data, uint16_t count, JsonObject &options)
{
    int i;

    for (i = 0; i < num_senders; i++)
    {
        if (sender_array[i].name == sender_name)
        {
            String payload = sender_array[i].sender->toFormattedString(sensor, data, count);
            if (sender_array[i].sender->sendPayload((uint8_t *)payload.c_str(), payload.length(), options))
            {
                ESP_LOGD(TAG,"Message sent by Sender: %s\n", sender_array[i].name.c_str());
                return true;
            }
            else
            {
                ESP_LOGD(TAG,"Failed to send message by Sender: %s\n", sender_array[i].name.c_str());
                return false;
            }
        }
    }
    ESP_LOGD(TAG,"Invalid sender name: %s\n", sender_name.c_str());
    return false;
}

void SenderContainer::loop()
{
    int i;

    for (i = 0; i < num_senders; i++)
    {
        sender_array[i].sender->loop();
    }
}