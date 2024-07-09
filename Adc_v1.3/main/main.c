#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "driver/adc.h"
#include "esp_system.h"

// Tamanho do buffer circular
#define BUFFER_SIZE 192
// Número de ciclos necessários para imprimir uma amostra no terminal
#define CYCLES_TO_SAMPLE 12

// Definição do handle do buffer circular
RingbufHandle_t adcBuffer;

// Contador de ciclos completos do buffer
int cycleCount = 0;

// Função para inicializar o ADC
void adc_init() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
}

// Função para inicializar o buffer circular
void buffer_init() {
    adcBuffer = xRingbufferCreate(BUFFER_SIZE * sizeof(uint16_t), RINGBUF_TYPE_NOSPLIT);
    if (adcBuffer == NULL) {
        printf("Failed to create ring buffer\n");
    }
}

// Tarefa para ler o ADC e armazenar os dados no buffer circular
void adc_task(void *pvParameter) {
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t sampling_period = pdMS_TO_TICKS(1); // 1 ms para 1000 SPS

    while (1) {
        uint16_t adcValue = adc1_get_raw(ADC1_CHANNEL_0);
        size_t item_size = sizeof(adcValue);
        BaseType_t result = xRingbufferSend(adcBuffer, &adcValue, item_size, pdMS_TO_TICKS(1));
        if (result != pdTRUE) {
            printf("Failed to send to ring buffer\n");
        }

        // Verificar se o buffer está cheio
        if (uxRingbufferGetCount(adcBuffer) == BUFFER_SIZE) {
            cycleCount++;
            if (cycleCount == CYCLES_TO_SAMPLE) {
                // Exibir amostra no terminal
                size_t item_size;
                uint16_t *sampleValue = (uint16_t *) xRingbufferReceive(adcBuffer, &item_size, pdMS_TO_TICKS(1));
                if (sampleValue != NULL) {
                    printf("Sampled ADC Value: %d\n", *sampleValue);
                    vRingbufferReturnItem(adcBuffer, (void *) sampleValue);
                }
                cycleCount = 0; // Reiniciar contagem de ciclos
            }
        }

        vTaskDelayUntil(&last_wake_time, sampling_period);
    }
}

// Função principal
void app_main() {
    // Inicializar buffer circular
    buffer_init();
    
    // Inicializar ADC
    adc_init();
    
    // Criar tarefa FreeRTOS para leitura do ADC
    xTaskCreate(&adc_task, "adc_task", 2048, NULL, 5, NULL);
}
