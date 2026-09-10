#ifndef TIMER_H
#define TIMER_H

#include "esp_timer.h"

/**
 * @brief Inicia um timer de alta resolução que incrementa uma variável global a cada segundo.
 */
void iniciar_timer();
void config_gpios(void);
extern volatile uint32_t contador_chamadas; // Variável global para monitoramento
#endif