#include <stdio.h>
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "rom/gpio.h"

#define OUT 22

void Timer_ISR_GPIO(void *param) {
    static bool level = false;
    gpio_set_level(OUT, level);
    level = !level;
}

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