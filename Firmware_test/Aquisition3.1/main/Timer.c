#include "libraries.h"                
#include "global.h"                   
#include "freertos/FreeRTOS.h"        
#include "freertos/semphr.h"          
#include "esp_task_wdt.h"             

// Semáforo para sincronizar o timer com a task principal
SemaphoreHandle_t timer_semaphore = NULL; 


void Timer_ISR(void *param) {

    Instantaneous_Energy();     // Executa aquisição instantânea de energia

    // Libera o semáforo para sinalizar a task principal
    if (timer_semaphore != NULL) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(timer_semaphore, &xHigherPriorityTaskWoken); // Libera semáforo dentro da ISR
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); // Troca de contexto se necessário
    }
}


void iniciar_timer(SemaphoreHandle_t semaphore) {
    timer_semaphore = semaphore; // Associa o semáforo recebido à variável global

    // Configuração dos parâmetros do timer
    const esp_timer_create_args_t my_timer_args = {
        .callback = &Timer_ISR,                // Função chamada a cada disparo do timer
        .name = "Meu Timer",                   // Nome do timer (para debug)
        .dispatch_method = ESP_TIMER_TASK      // Método de despacho (executa em task dedicada)
    };

    esp_timer_handle_t timer_handler;          // Handler para o timer

    ESP_ERROR_CHECK(esp_timer_create(&my_timer_args, &timer_handler)); // Cria o timer
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 1042));    // Inicia timer  (aprox. 1ms para 1kHz)
}