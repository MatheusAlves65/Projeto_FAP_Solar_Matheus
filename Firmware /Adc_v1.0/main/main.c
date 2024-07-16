#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "adc_calc.h"

void app_main(void) {
    init_adc();
    
    while (1) {
        int adc_value = adc1_get_raw(ADC_CHANNELvoltage);
        float output_voltage = adc_to_voltage(adc_value);
        printf("ADC Value: %d, Output Voltage: %.2f V\n", adc_value, output_voltage);
        
        // Atraso de 1 segundo
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
