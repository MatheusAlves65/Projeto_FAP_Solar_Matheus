//#include "libraries.h"                
#include "freertos/FreeRTOS.h"        
#include "freertos/semphr.h"          
#include "esp_task_wdt.h"      
#include "esp_timer.h"
#include "Monitoring.h"
#include "driver/gpio.h"
#define pino_osciloscopio 19 // Pino para teste de osciloscópio (GPIO2)
// Semáforo para sincronizar o timer com a task principal
SemaphoreHandle_t timer_semaphore = NULL; 
volatile int64_t pior_tempo_execucao_adc = 0;



void Timer_ISR(void *param) {

    int64_t tempo_inicio = esp_timer_get_time();
    Instantaneous_Energy();     // Executa aquisição instantânea de energia

    int64_t tempo_fim = esp_timer_get_time();
    int64_t duracao = tempo_fim - tempo_inicio;

    // 3. Salva apenas o pior caso observado
    if (duracao > pior_tempo_execucao_adc) {
        pior_tempo_execucao_adc = duracao;
    }
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
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer_handler, 1041));    // Inicia timer  (aprox. 1ms para 960Hz)
}