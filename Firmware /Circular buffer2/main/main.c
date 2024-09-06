#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "circular_buffer.h"

static const char *TAG = "CircularBufferTest";
#define ADC_CHANNEL ADC1_CHANNEL_7 // Ajuste o canal ADC conforme necessário
#define SAMPLE_RATE 960  // samples per second
#define RMS_CALC_INTERVAL 200  // Intervalo para cálculo RMS em ms

void buffer_test_task(void *pvParameter) {
    ESP_LOGI(TAG, "Iniciando coleta de dados do ADC.");
    circular_buffer_t buffer1;  // Declare buffer1
    circular_buffer_t buffer2;  // Declare buffer2
    buffer_init(&buffer1, BUFFER_SIZE_1, BUFFER_SIZE_1);
    buffer_init(&buffer2, BUFFER_SIZE_2, BUFFER_SIZE_2);
    for (int i = 0; i < BUFFER_SIZE_1; i++) {
        int adc_value = adc1_get_raw(ADC_CHANNEL);
        ESP_LOGI(TAG, "Valor ADC: %d", adc_value);  // Log do valor ADC
        buffer_write_1(&buffer1, adc_value);
        vTaskDelay(pdMS_TO_TICKS(1000 / SAMPLE_RATE));
    }
    ESP_LOGI(TAG, "Coleta de dados do ADC concluída.");

    // Calcula RMS e insere no buffer 2
    float rms_value = calculate_rms(&buffer1);
    buffer_write_2(&buffer2, rms_value);

    // Lê e registra todos os valores do buffer 2
    if (!buffer_is_empty_2(&buffer2)) {
        ESP_LOGI(TAG, "RMS Buffer Values:");
        for (int i = 0; i < buffer2.count; i++) {
            float value = buffer_read_2(&buffer2);
            ESP_LOGI(TAG, "RMS Value: %f", value);
            vTaskDelay(pdMS_TO_TICKS(200));
        }
    }

    vTaskDelay(pdMS_TO_TICKS(RMS_CALC_INTERVAL));  // Espera antes do próximo cálculo RMS
}

void app_main() {
    // Configuração do ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN_DB_0);

    xTaskCreate(&buffer_test_task, "buffer_test_task", 4096, NULL, 5, NULL);
}

