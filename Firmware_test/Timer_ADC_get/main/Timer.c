#include "libraries.h"
#include "global.h"

volatile uint32_t contador_chamadas = 0; // Contador global para monitoramento

void Timer_ISR(void *param) {
    adc_init_Reading(); // Atualiza os valores da variável global "medidas"
}

void iniciar_timer(void) {

    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR,
        .name = "Meu Timer"
    };

    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 2000000)); // 500 microsegundos
}

