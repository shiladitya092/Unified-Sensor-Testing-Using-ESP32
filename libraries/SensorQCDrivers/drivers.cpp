#include <Arduino.h>
#include "drivers.h"
#ifdef SENSOR_DHTSensor
    #include "DHTSensor.h"
#endif
#ifdef SENSOR_LTSensor
    #include "DS18B20.h"
#endif
#ifdef SENSOR_SPIFFSSensor
    #include "spiffs_sensor.h"
#endif
#ifdef SENSOR_ProbeSensor
    #include "ProbeSensor.h"
#endif
#ifdef SENSOR_MoistureSensor
    #include "moisture_sensor.h"
#endif
#ifdef SENSOR_MultiProbeSensor
    #include "MultiProbeSensor.h"
#endif
#ifdef SENSOR_ADS7844
    #include "ADS7844.h"
#endif
#ifdef SENSOR_MDHRegister
    #include "mdh_register.h"
#endif

void *createSensorObject(String sensor_class)
{
    void *sensor;
    if (false)
    {
    }
#ifdef SENSOR_DHTSensor
    else if (sensor_class == "DHTSensor")
    {
        sensor = new DHTSensor();
    }
#endif
#ifdef SENSOR_LTSensor
    else if (sensor_class == "LTSensor")
    {
        sensor = new LTSensor();
    }
#endif
#ifdef SENSOR_SPIFFSSensor
    else if (sensor_class == "SPIFFSSensor")
    {
        sensor = new SPIFFSSensor();
    }
#endif
#ifdef SENSOR_ProbeSensor
    else if (sensor_class == "ProbeSensor")
    {
        sensor = new ProbeSensor();
    }
#endif
#ifdef SENSOR_MoistureSensor
    else if (sensor_class == "MoistureSensor")
    {
        sensor = new MoistureSensor();
    }
#endif
#ifdef SENSOR_MultiProbeSensor
    else if (sensor_class == "MultiProbeSensor")
    {
        sensor = new MultiProbeSensor();
    }
#endif
#ifdef SENSOR_ADS7844
    else if (sensor_class == "ADS7844")
    {
        sensor = new ADS7844();
    }
#endif
#ifdef SENSOR_MDHRegister
    else if (sensor_class = "MDHRegister")
    {
        sensor = new MDHRegister();
    }
#endif
    else
    {
        sensor = NULL;
    }
    return sensor;
}