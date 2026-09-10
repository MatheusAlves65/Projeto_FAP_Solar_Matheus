#include "Events.h"
#include "esp_log.h"
#include "esp_timer.h" // Para o esp_timer_get_time()
#include <time.h>

QueueHandle_t fila_eventos = NULL;

void Inicializar_Fila_Eventos(void) {
    // Cria uma fila com capacidade para armazenar 200 amostras simultâneos
    fila_eventos = xQueueCreate(10, sizeof(PacoteEvento_t));
    
    if (fila_eventos == NULL) {
        ESP_LOGE("EVENTOS", "Falha ao criar fila de eventos! RAM insuficiente?");
    } else {
        ESP_LOGI("EVENTOS", "Fila de eventos inicializada com sucesso.");
    }
}



bool Enviar_Evento_Fila(PacoteEvento_t *pacote) {
    if (fila_eventos == NULL) return false;

    // Tenta enviar para a Fila
    if (xQueueSend(fila_eventos, pacote, 0) == pdPASS) {
        return true;
    } 
    return false;
    
    // Se a fila estiver cheia (MQTT travado ou sem Wi-Fi), o pacote é descartado em silêncio.
    return false;
}