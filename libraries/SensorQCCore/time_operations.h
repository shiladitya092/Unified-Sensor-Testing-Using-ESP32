#ifndef TIME_OPERATIONS_H
#define TIME_OPERATIONS_H

typedef struct time_data_t
{
    bool initialized;
} time_data_t;

extern time_data_t time_data;

void setup_time();
unsigned long long get_soft_time_us();
unsigned long long get_soft_time_ms();
unsigned long long get_soft_time_s();

#endif