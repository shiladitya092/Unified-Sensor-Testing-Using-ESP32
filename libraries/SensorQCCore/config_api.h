#ifndef CONFIG_API_H
#define CONFIG_API_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void register_config_api(AsyncWebServer &webSvr_handle);

#endif