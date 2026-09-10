#ifndef MQTT_H
#define MQTT_H
#include "Events.h"

void mqtt_start(void);
void mqtt_enviar_rotina(float tensao, float corrente, float temperatura);
void mqtt_enviar_alerta(PacoteEvento_t pacote);
void mqtt_alerta_on(int64_t timestamp, const char* tipo);
void mqtt_alerta_off(int64_t timestamp_fim, int64_t duracao_ms, const char* tipo);
#endif