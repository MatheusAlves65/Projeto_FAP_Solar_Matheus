#include "libraries.h"
#include "global.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h" // Biblioteca para ESP_LOG

extern volatile uint32_t contador_chamadas; // Declaração do contador global

void voltage_read(void *param) {
    ESP_LOGD("Tasks", "Task Voltage Read iniciando...\n");

    // Cria o semaphore binário
    SemaphoreHandle_t timer_semaphore = xSemaphoreCreateBinary();
    if (timer_semaphore == NULL) {
        ESP_LOGE("Tasks", "Falha ao criar o semaphore.");
        vTaskDelete(NULL);
    }

    // Inicialização do ADC
    adc_setup();

    // Inicia o Timer e passa o semaphore
    iniciar_timer(timer_semaphore);

    while (1) {
        // Aguarda o semaphore ser liberado pelo timer
        if (xSemaphoreTake(timer_semaphore, portMAX_DELAY) == pdTRUE) {
            // Verifica se o buffer foi processado
            if (buffer_completo == true) {
                After_Acquisition();
                buffer_completo = false; // Reset do flag após processamento
                //ESP_LOGI("Tasks", "After_Acquisition executada e buffer_completo resetado");
            }
        }
    }
}