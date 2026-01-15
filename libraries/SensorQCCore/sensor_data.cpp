#include <Arduino.h>
#include "sensor_data.h"

typedef void (*print_func_t)(void *data);
typedef void (*delete_func_t)(void *data);

SensorData::SensorData()
{
    front = -1;
    rear = -1;
    data = NULL;
}

SensorData::~SensorData()
{
    free(data);
    data = NULL;
}

bool SensorData::init(int size)
{
    len = size;
    if (data == NULL)
    {
        free(data);
    }
    data = (void **)calloc(size, sizeof(void *));
    if (!data)
    {
        return false;
        ESP_LOGD(TAG, "Error allocating memory!\n");
    }
    front = -1;
    rear = -1;
    return true;
}

bool SensorData::add(void *value)
{
    if (front == len - 1)
        return false;
    if (front == -1)
        front = rear = 0;
    else
        rear = (rear + 1);
    data[rear] = value; // Revisit this if structure has pointers/mem allocs
    return true;
}

int SensorData::getN(int count, void ***store)
{
    int count_temp = count;
    if (count > (rear + 1))
        count_temp = rear + 1;
    *store = data;
    // memcpy(store, *data, count_temp * sizeof(void *));
    return count_temp;
}

int SensorData::getCount()
{
    return rear + 1;
}

void *SensorData::getLast()
{
    if (front != -1)
    {
        return data[front];
    }
    else
    {
        return NULL;
    }
}

void SensorData::printAll(print_func_t print_func)
{
    if (front == -1)
        return;
    for (int i = 0; i < rear; i++)
    {
        print_func(data[i]);
        ESP_LOGD(TAG, " ");
    }
    ESP_LOGD(TAG, "\n");
}

bool SensorData::del(delete_func_t delete_func)
{
    int i;
    if (data != NULL)
    {
        for (i = 0; i < getCount(); i++)
        {
            if (data[i])
                delete_func(data[i]);
        }
        // free(data);
        // data = NULL;
    }
}

bool SensorData::reset()
{
    // free(data);
    front = rear = -1;
}
