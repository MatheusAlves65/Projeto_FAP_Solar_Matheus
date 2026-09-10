#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/* ========================================================================
 * ESTRUTURA DO PACOTE DE EVENTO
 * ======================================================================== */
#define NUM_amostras 200
/**
 * @brief Estrutura do pacote de dados a ser enviado para a Fila (Queue)
 */
typedef struct {
    int64_t timestamp;
    float tensao[NUM_amostras];
    float corrente[NUM_amostras];
    const char* tipo;
} PacoteEvento_t;

/* ========================================================================
 * VARIÁVEIS GLOBAIS
 * ======================================================================== */

// Handle global da fila (acessível por main.c, mqtt.c, etc)
extern QueueHandle_t fila_eventos;

/* ========================================================================
 * FUNÇÕES PÚBLICAS
 * ======================================================================== */
/**
 * @brief Cria a Fila do FreeRTOS na memória. 
 * Deve ser chamada no app_main() antes de criar as tarefas.
 */
void Inicializar_Fila_Eventos(void);

/**
 * @brief Empacota os dados e tenta enviar para a fila do MQTT.
 * Já possui um "freio de segurança" interno para não sobrecarregar a rede.
 * * @param tensao Valor médio da tensão no momento do evento
 * @param corrente Valor médio da corrente no momento do evento
 * @param temperatura Valor médio da temperatura no momento do evento
 * @return true se conseguiu enviar para a fila, false se foi bloqueado ou fila cheia.
 */
bool Enviar_Evento_Fila(PacoteEvento_t *pacote);

#endif /* EVENTS_H */