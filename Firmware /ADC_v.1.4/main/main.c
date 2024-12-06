#include <stdio.h>
#include <driver/adc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Setup_adc.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"

float tensao_pos_func;
// Tamanho do buffer em bytes
#define BUFFER_SIZE 768

// Ponteiro para o ringbuffer
static RingbufHandle_t ringbuf;

void writer_task(void *param) {
    float adc_data[192]; // Array to store 48 float values (192 bytes)
    while (1) {
        for (int i = 0; i < 192; i++) {
            tensao_pos_func = adc_init_voltage(tensao_pos_func);
            adc_data[i] = tensao_pos_func; // Leitura do ADC
        }

        // Enviar dados para o ringbuffer
        if (xRingbufferSend(ringbuf, adc_data, sizeof(adc_data), pdMS_TO_TICKS(1000)) != pdTRUE) {
            printf("Falha ao enviar dados para o ringbuffer\n");
        
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 2 segundos antes de enviar novamente
    }
}
// Tarefa para ler do ringbuffer
void reader_task(void *param) {
    size_t item_size;
    float *received_data;

    while (1) {
        // Receber dados do ringbuffer
        received_data = (float *)xRingbufferReceive(ringbuf, &item_size, pdMS_TO_TICKS(1000));
        if (received_data != NULL) {
            printf("Dados recebidos do ringbuffer:\n");
            for (int i = 0; i < item_size / sizeof(float); i++) {
                printf("%f ", received_data[i]);
            }
            printf("\n");

            // Liberar a memória alocada pelo ringbuffer
            vRingbufferReturnItem(ringbuf, (void *)received_data);
        } else {
            printf("Nenhum dado recebido\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Espera 2 segundos antes de tentar ler novamente
    }
}




//float adc_init_voltage(float tensao_pos_func);
//void adc_init_current();
//void adc_init_temp();


void app_main(void)
{
    
ringbuf = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    if (ringbuf == NULL) {
        printf("Falha ao criar o ringbuffer\n");
        return;
    }
    xTaskCreate(writer_task, "writer_task", 2048, NULL, 1, NULL);
    xTaskCreate(reader_task, "reader_task", 2048, NULL, 1, NULL);


    while (1) {
         //tensao_pos_func = adc_init_voltage(tensao_pos_func);
         //adc_init_current();
         //adc_init_temp();
   // printf("Tensão pós func:%2.f\n", tensao_pos_func) ;   
        // Aguardar um pouco antes de ler novamente
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}