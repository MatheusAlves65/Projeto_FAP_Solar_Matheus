#ifndef TIMER_H
#define TIMER_H

#include "esp_timer.h"

// Função para inicializar o timer
void iniciar_timer(float (*callback)(void));
extern volatile uint32_t contador_chamadas; // Variável global para monitoramento
#endif