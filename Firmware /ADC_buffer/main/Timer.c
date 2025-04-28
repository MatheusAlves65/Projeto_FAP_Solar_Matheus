#include "libraries.h"
#include "global.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

volatile uint32_t contador_chamadas = 0; // Contador global para monitorar chamadas do timer
SemaphoreHandle_t timer_semaphore = NULL; // Semaphore para sincronizar o timer com a task

void Timer_ISR(void *param) {
    Instant_Acquisition(); // Atualiza os valores da variável global "medidas"

    // Incrementa o contador de chamadas
    contador_chamadas++;

    // Libera o semaphore para a task
    if (timer_semaphore != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(timer_semaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void iniciar_timer(SemaphoreHandle_t semaphore) {
    timer_semaphore = semaphore; // Armazena o semaphore para uso no ISR

    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR,
        .name = "Meu Timer"
    };

    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 100000)); // 100 ms
}