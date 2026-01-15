
#include <Arduino.h>
#include "sensor_data.h"
#include "phase.h"



    Phase::Phase()
    {
    }

    Phase::~Phase()
    {
    }

    void Phase::init(phase_opts_t phase_opts)
    {
        gather = phase_opts.gather;
        autonext = phase_opts.autonext;
        duration = phase_opts.duration;
        sampling_rate = phase_opts.sampling_rate;
        message = String(phase_opts.message);
    }

    bool Phase::update(uint16_t new_duration)
    {
        duration = new_duration;
        if (gather)
            return data_store.init((duration * sampling_rate) + 1); // +1?
        return true;
    }

    bool Phase::add(void *data)
    {
        return data_store.add(data);
    }

    int Phase::getN(uint16_t count, void ***data_array)
    {
        int ret_count = data_store.getN(count, data_array);
        return ret_count;
    }

    int Phase::getCount()
    {
        return data_store.getCount();
    }

    void* Phase::getLast()
    {
        return data_store.getLast();
    }

    bool Phase::delete_data(delete_func_t delete_func)
    {
        data_store.del(delete_func);
    }

    bool Phase::reset()
    {
        data_store.reset();
    }

    bool Phase::getGather()
    {
        return gather;
    }

    bool Phase::getAutoNext()
    {
        return autonext;
    }

    unsigned long Phase::getDuration()
    {
        return duration;
    }

    void Phase::setStart()
    {
        start_time = millis();
    }

    bool Phase::isCheckpoint(unsigned long last_read_time)
    {
        if (millis() - last_read_time >= 1000.0 / sampling_rate)
            return true;
        return false;
    }

    bool Phase::isComplete()
    {
        // if ((millis() - start_time) / 1000 >= duration)
        // {
        //     return true;
        // }
        // else
        // {
        //     return false;
        // }
        if (getCount() == (int)(duration * sampling_rate))
            return true;
        return false;
    }

    void Phase::printAllData(print_func_t print_func)
    {
        data_store.printAll(print_func);
    }

    String Phase::getMessage()
    {
        return message;
    }
