#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"

// Tamanho do buffer em bytes
#define BUFFER_SIZE 192

// Ponteiro para o ringbuffer
static RingbufHandle_t ringbuf;

// Tarefa para escrever no ringbuffer
void writer_task(void *param) {
    float adc_data[BUFFER_SIZE / sizeof(float)];
    for (int i = 0; i < BUFFER_SIZE / sizeof(float); i++) {
        adc_data[i] = (float)i; // Simulação de dados do ADC
    }
    while (1) {
        // Enviar dados para o ringbuffer
        if (xRingbufferSend(ringbuf, adc_data, sizeof(adc_data), pdMS_TO_TICKS(1000)) != pdTRUE) {
            printf("Falha ao enviar dados para o ringbuffer\n");
        } else {
            printf("Dados enviados:\n");
        vTaskDelay(pdMS_TO_TICKS(2000)); // Espera 2 segundos antes de enviar novamente
    }
}}

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
        vTaskDelay(pdMS_TO_TICKS(2000)); // Espera 2 segundos antes de tentar ler novamente
    }
}

void app_main() {
    // Criar o ringbuffer
    ringbuf = xRingbufferCreate(BUFFER_SIZE, RINGBUF_TYPE_BYTEBUF);
    if (ringbuf == NULL) {
        printf("Falha ao criar o ringbuffer\n");
        return;
    }

    // Obter e imprimir o tamanho real do buffer
    size_t max_item_size = xRingbufferGetMaxItemSize(ringbuf);
    size_t free_size = xRingbufferGetCurFreeSize(ringbuf);
    printf("Tamanho máximo do item no ringbuffer: %d bytes\n", max_item_size);
    printf("Espaço livre no ringbuffer: %d bytes\n", free_size);

    // Criar as tarefas
    xTaskCreate(writer_task, "writer_task", 2048, NULL, 1, NULL);
    xTaskCreate(reader_task, "reader_task", 2048, NULL, 1, NULL);
}