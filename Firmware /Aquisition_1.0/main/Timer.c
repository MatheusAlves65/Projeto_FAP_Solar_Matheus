#include "libraries.h"
#include "global.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_task_wdt.h"

// Add volatile flag to prevent multiple concurrent executions


SemaphoreHandle_t timer_semaphore = NULL; // Semaphore para sincronizar o timer com a task

void Timer_ISR(void *param) {

    
    Instant_Acquisition();

    if (timer_semaphore != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(timer_semaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    
   
}

void iniciar_timer(SemaphoreHandle_t semaphore) {
    timer_semaphore = semaphore;
    

    
    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR,
        .name = "Meu Timer",
        .dispatch_method = ESP_TIMER_TASK  // Explicitly set dispatch method
    };

    esp_timer_handle_t timer_handler;
    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 1042)); // 1ms period for 1kHz
}