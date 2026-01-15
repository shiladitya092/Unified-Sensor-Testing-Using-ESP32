#include <Arduino.h>
#include "senders.h"
#include "http_sender.h"
#include "serial_writer.h"
#include "spiffs_file_writer.h"


void* createSenderObject(String sensor_class)
{
    void *sender;
    if (sensor_class == "HttpSender")
    {
        sender = new HttpSender();
    }
    else if(sensor_class == "HttpsSender")
    {
        sender = new HttpsSender();
    }
    else if(sensor_class == "SpiffsFileWriter")
    {
        sender = new SpiffsFileWriter();
    }
    else if(sensor_class == "SerialWriter")
    {
        sender = new SerialWriter();
    }
    else
    {
        sender = NULL;
    }
    return sender;
}