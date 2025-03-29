#include "libraries.h"
#include "global.h"

// Ponteiro para o ringbuffer
static RingbufHandle_t ringbuf;

// Declare a variável como extern para evitar a definição múltipla
//extern TaskHandle_t Handle_voltage_read;

void writer_task(void *param) {
    float adc_data[192]; // Array to store 48 float values (192 bytes)
    while (1) {
        for (int i = 0; i < 192; i++) {
            //tensao_pos_func = adc_init_voltage(tensao_pos_func);
            //adc_data[i] = tensao_pos_func; // Leitura do ADC
        }

        // Enviar dados para o ringbuffer
        if (xRingbufferSend(ringbuf, adc_data, sizeof(adc_data), pdMS_TO_TICKS(1000)) != pdTRUE) {
            printf("Falha ao enviar dados para o ringbuffer\n");
        
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 2 segundos antes de enviar novamente
    }
}


void voltage_read(void *param) {
    printf("Task Voltage Read iniciando...\n");

    // Iniciar o Timer que chamará adc_init_Reading periodicamente
    iniciar_timer();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo

        // Exibe os valores mais recentes da medição
        printf("Tensão medida: %.2f \n", medidas.tensao_medida);
        printf("Corrente medida: %.2f \n", medidas.corrente_medida);
    }
}