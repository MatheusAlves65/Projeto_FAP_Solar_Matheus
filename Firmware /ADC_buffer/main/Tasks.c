#include "libraries.h"
#include "global.h"




void voltage_read(void *param) {
    printf("Task Voltage Read iniciando...\n");

    // Iniciar o Timer que chamará adc_init_Reading periodicamente
    iniciar_timer();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo
        //sdks

        // Exibe os valores mais recentes da mediçã
    }
}