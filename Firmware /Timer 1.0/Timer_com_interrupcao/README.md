# Explicação do Código
 Este código é um exemplo de um programa para um microcontrolador ESP32 que utiliza um timer para alternar o estado de um pino GPIO periodicamente. Aqui está uma explicação detalhada:

## Inclusão de Bibliotecas
Estas bibliotecas fornecem funções e definições necessárias para trabalhar com timers, GPIOs e o sistema FreeRTOS no ESP32.
```c
#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
```

## Definição de Constantes
Define o pino GPIO 22 como a saída que será controlada pelo timer.
```c
#define OUT 22
```

## Função de Interrupção do Timer
Esta função é chamada cada vez que o timer dispara. Ela alterna o estado do pino GPIO 22 entre alto e baixo.
```c
void Timer_ISR_GPIO(void *param) {
    static bool level = false;
    gpio_set_level(OUT, level);
    level = !level;
}
```


## Função Principal (app_main)
Configuração do GPIO:
```c
void app_main(void) {
    gpio_pad_select_gpio(OUT);
    gpio_set_direction(OUT, GPIO_MODE_OUTPUT);

    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR_GPIO,
        .name = "My Timer"
    };
    esp_timer_handle_t timer_handler;

    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 500)); // 500 microseconds

    while (true) {
        esp_timer_dump(stdout);
    }
}
```
### Configuração do GPIO:
Configura o pino GPIO 22 como uma saída.
```c
gpio_pad_select_gpio(OUT);
gpio_set_direction(OUT, GPIO_MODE_OUTPUT);
```

## Criação e Início do Timer:
```c
const esp_timer_create_args_t my_timer_args = {
    .callback = &Timer_ISR_GPIO,
    .name = "My Timer"
};
esp_timer_handle_t timer_handler;

ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 500)); // 500 microseconds


```
Define os argumentos do timer, cria o timer e o inicia com um período de 500 microsegundos.

## Loop Infinito:

Em um loop infinito, imprime informações sobre os timers ativos no console.
```c
while (true) {
    esp_timer_dump(stdout);
}
```


## Resumo
Este código configura um timer periódico que alterna o estado de um pino GPIO a cada 500 microsegundos e imprime informações sobre os timers no console continuamente. Gerando uma onda quadrada de 1kHz.
