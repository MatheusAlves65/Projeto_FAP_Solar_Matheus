#include <stdio.h>
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Setup_adc.h"



void app_main(void)
{
    
    while (1) {
        void adc_init_voltage();
        void adc_init_current();
        void adc_init_temp();

        // Aguardar um pouco antes de ler novamente
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}