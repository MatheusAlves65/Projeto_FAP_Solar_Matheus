#include <stdio.h>
#include <stdint.h>
#include "Energy.h"
#include "esp_log.h"
#include "Monitoring.h"
#include "Events.h"
#include <stdbool.h>
#include <driver/adc.h>
#include "Temp.h"
#include "MOSFET.h"
#include "Events.h"
#include "time.h"
#include <sys/time.h>
#include "esp_timer.h"



volatile bool event_flag_3s = false; // Flag para sinalizar que o terceiro nível foi atualizado, para verificação de eventos na tarefa principal
volatile bool buffer_completo = false; // Definição da variável global
volatile bool log_pendente;
int contador_eventos = 0; // Contador de eventos de limite superior e inferior

 Energy_ADC Voltage = {.adc_channel = ADC1_CHANNEL_3, .tipo = "Tensao", 
    .max_threshold = 30, .min_threshold = 25,  .offset = -0.01f, .ganho = 27.30f, 
    .tempo_confirmacao_ms = 5, .contador_amostras = 0, .MOSFET_active_ms=0,.tempo_acionamento_mosfet_ms=50};
 Energy_ADC Current = {.adc_channel = ADC1_CHANNEL_2, .tipo = "Corrente",
    .max_threshold = 15, .min_threshold = 13, .offset = -0.23f, .ganho = 6.72f,
    .tempo_confirmacao_ms = 5, .contador_amostras = 0,.MOSFET_active_ms=0,.tempo_acionamento_mosfet_ms=50};
 Temp_ADC Temperatura = {.tipo = "Temperatura", .offset = 94.85f, .ganho = -39.73f};


static PacoteEvento_t pacote_evento;
static int indice_amostra = 0;
volatile bool gravando_evento = false;



void Instantaneous_Energy() { // primeiro nivel do buffer
    Instant_Acquisition(&Voltage); // Aquisição instantânea de energia para a tensão
    Instant_Acquisition(&Current); // Aquisição instantânea de energia para a corrente


    // 1. Início do evento: Verifica o gatilho, define o tipo UMA VEZ e começa a gravar
    if ((Voltage.state == false || Current.state == false) && !gravando_evento) {
        gravando_evento = true;
        indice_amostra = 0;
        
        // Define quem é o "culpado" pelo evento no momento do disparo
        if (Voltage.state == false && Current.state == false) {
            pacote_evento.tipo = "Tensao_e_Corrente"; // Opcional, caso ambos desarmem juntos
        } else if (Voltage.state == false) {
            pacote_evento.tipo = Voltage.tipo;
        } else if (Current.state == false) {
            pacote_evento.tipo = Current.tipo;
        }
        else {
            pacote_evento.tipo = "Desconhecido"; // Fallback, não deveria acontecer
        }

        struct timeval tv;
        gettimeofday(&tv, NULL);
        pacote_evento.timestamp = (int64_t)tv.tv_sec * 1000LL + (int64_t)tv.tv_usec / 1000LL;
        
        ESP_LOGW("GRAVADOR", "Iniciando captura de %d amostras. Motivo: %s", NUM_amostras, pacote_evento.tipo);
    }

    // 2. Se a gravação estiver ativada, salva os dados incondicionalmente
    if (gravando_evento) {
        // Grava sempre ambas as grandezas, sem checar os estados de novo e sem colocar zeros!
        pacote_evento.tensao[indice_amostra] = Voltage.instant_value;
        pacote_evento.corrente[indice_amostra] = Current.instant_value;
        
        indice_amostra++;

        // 3. Quando encher as posições (chegou em NUM_amostras)
        if (indice_amostra >= NUM_amostras) {
            Enviar_Evento_Fila(&pacote_evento);
            gravando_evento = false; // Desativa até o próximo evento acontecer
            ESP_LOGW("GRAVADOR", "Captura concluída! Pacote na Fila.");
        }
    }
}

void Continuous_Energy(){
   After_Acquisition(&Voltage);
    After_Acquisition(&Current);
   ler_temperatura_adc(&Temperatura);
}

void Initialize_Parameters(){
    Energy_ADC_Init(&Voltage);
    Energy_ADC_Init(&Current);
}


