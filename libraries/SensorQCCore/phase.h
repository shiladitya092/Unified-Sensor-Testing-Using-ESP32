#ifndef PHASE_H
#define PHASE_H

#include <Arduino.h>
#include "sensor_data.h"

typedef struct phase_opts_t
{
    bool gather;
    bool autonext;
    uint16_t duration;   // seconds
    float sampling_rate; // hertz
    String message;
} phase_opts_t;

class Phase
{
private:
    SensorData data_store;
    bool gather;
    bool autonext;
    uint16_t duration;        // seconds
    float sampling_rate;      // hertz
    unsigned long start_time; // milliseconds
    String message;

public:
    Phase();
   ~Phase();
    void init(phase_opts_t phase_opts);
    bool update(uint16_t new_duration);
    bool add(void *data);
    int getN(uint16_t count, void ***data_array);
    int getCount();
    void* getLast();
    bool delete_data(delete_func_t delete_func);
    bool reset();
    bool getGather();
    bool getAutoNext();
    unsigned long getDuration();
    void setStart();
    bool isCheckpoint(unsigned long last_read_time);
    bool isComplete();
    void printAllData(print_func_t print_func);
    String getMessage();
};

#endif