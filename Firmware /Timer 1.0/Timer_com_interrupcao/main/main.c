#include "driver/gpio.h"
#include "rom/gpio.h"

#define OUT 22 // Define o pino GPIO 22 como saída

// Função de callback do timer, chamada a cada período do timer
void Timer_ISR_GPIO(void *param) {
    static bool level = false; // Variável estática para armazenar o nível atual do GPIO
    gpio_set_level(OUT, level); // Define o nível do GPIO
    level = !level; // Alterna o nível do GPIO
}

void app_main(void) {
    gpio_pad_select_gpio(OUT); // Seleciona o pino GPIO 22
    gpio_set_direction(OUT, GPIO_MODE_OUTPUT); // Define o pino GPIO 22 como saída

    // Configura os argumentos para a criação do timer
    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR_GPIO, // Define a função de callback do timer
        .name = "My Timer" // Nome do timer
    };
    esp_timer_handle_t timer_handler; // Manipulador do timer

    // Cria o timer com os argumentos especificados
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    // Inicia o timer em modo periódico com um período de 500 microsegundos
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 500)); // 500 microsegundos

    // Loop infinito para manter o programa rodando
    while (true) {
        esp_timer_dump(stdout); // Imprime informações do timer no console
    }
}