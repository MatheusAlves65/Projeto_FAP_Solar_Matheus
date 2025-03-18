#include <stdio.h>
#include "Timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "Timer.h"
#include <stdio.h>
#include "esp_log.h"

volatile uint32_t contador_chamadas = 0; // Contador global para monitoramento
static float (*funcao_callback)(void) = NULL; // Ponteiro para a função de callback

void Timer_ISR(void *param) {
    contador_chamadas++; // Incrementa o contador a cada chamada do timer   
    printf("Chamadas do timer: %lu\n", contador_chamadas);
    if (funcao_callback != NULL) {
        funcao_callback(); // Chama a função definida na main.c
    }
}

void iniciar_timer(float (*callback)(void)) {
    funcao_callback = callback; // Salva o ponteiro da função passada

    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR,
        .name = "Meu Timer"
    };

    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 200000)); // 500 microsegundos
}

