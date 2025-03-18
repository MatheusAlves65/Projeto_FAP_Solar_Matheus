#include <stdio.h>
#include "Timer.h"
#include <stdio.h>
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Setup_adc.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"

int i = 0;


TaskHandle_t Handle_voltage_read;
TaskHandle_t Handle_current_read;

void current_read(void *param) {
    while(1) {
        printf("Task Current Read rodando...\n");
        iniciar_timer(adc_init_current);
        vTaskDelay(pdMS_TO_TICKS(500));
        vTaskSuspend(Handle_current_read);  
    }
}

void voltage_read(void *param) {
    printf("Task Voltage Read iniciando...\n");
    iniciar_timer(adc_init_voltage);
    vTaskSuspend(Handle_voltage_read);  
}



void app_main(void)
{

xTaskCreate(voltage_read, "voltage_read", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_voltage_read);
xTaskCreate(voltage_read, "voltage_read", 2048, NULL, tskIDLE_PRIORITY + 2, &Handle_current_read);
return;



while (1) { 
    vTaskSuspend(NULL);
   vTaskDelay(pdMS_TO_TICKS(1000));
}

}