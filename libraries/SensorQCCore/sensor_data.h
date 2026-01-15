#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

typedef void (*print_func_t)(void *data);
typedef void (*delete_func_t)(void *data);

class SensorData
{
private:
    void **data;
    int len;
    int front, rear;

public:
    SensorData();
    ~SensorData();
    bool init(int size);
    bool add(void *value);
    int getN(int count, void ***store);
    int getCount();
    void *getLast();
    void printAll(print_func_t print_func);
    bool del(delete_func_t delete_func);
    bool reset();
};

#endif