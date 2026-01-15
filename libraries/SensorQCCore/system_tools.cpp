#include <Arduino.h>
#include "system_tools.h"


bool keep_running = true;

void espRestartTask(void *pvParameters)
{
    int wait_time = 200;
    ESP_LOGD(TAG,"Restarting ESP in %d millis...\n", wait_time);
    vTaskDelay(wait_time);
    ESP.restart();
    vTaskDelete(NULL);
}

void espRestart()
{
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;

    keep_running = false;
    xReturned = xTaskCreate(
        espRestartTask,   /* Function that implements the task. */
        "ESP_RESTART",    /* Text name for the task. */
        10000,            /* Stack size in words, not bytes. */
        NULL,             /* Parameter passed into the task. */
        tskIDLE_PRIORITY, /* Priority at which the task is created. */
        &xHandle);        /* Used to pass out the created task's handle. */
}