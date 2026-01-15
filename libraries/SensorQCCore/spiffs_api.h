#ifndef SPIFFS_API_H
#define SPIFFS_API_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>


void register_spiffs_api(AsyncWebServer &webSvr_handle);

#endif