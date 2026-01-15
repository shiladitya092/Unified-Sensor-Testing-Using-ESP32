#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <Arduino.h>
#include <ArduinoJson.h>


class Configurator
{
private:
    String config_file;

public:
    DynamicJsonDocument config_json;
    Configurator();
    ~Configurator();
    void init(String config_file);
    bool loadConfigs();
    bool saveConfigs();
    bool mergeConfig(String new_config_string);
};

extern Configurator global_configurator;
extern Configurator wifi_configurator;

#endif