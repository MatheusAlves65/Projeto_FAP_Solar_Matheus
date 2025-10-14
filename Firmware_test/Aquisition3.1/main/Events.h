#ifndef EVENTS_H
#define EVENTS_H

#include <stdbool.h>
#include "esp_err.h"



/* ========================================================================
 * CONSTANTES E DEFINIÇÕES
 * ======================================================================== */

/**
 * @brief Número de ciclos de monitoramento após o evento terminar
 * 
 * Define quantos ciclos de medição serão realizados após o evento
 * analógico finalizar, para capturar o regime permanente do sistema.
 * 
 * Exemplo: Se cada ciclo é 3 segundos e o valor é 10, 
 * monitora por mais 30 segundos após o evento.
 */
#ifndef MEASUREMENT_AFTER_ANALOG_EVENT
#define MEASUREMENT_AFTER_ANALOG_EVENT 35
#endif

/**
 * @brief Namespace usado no NVS para armazenar dados de eventos
 */
#define EVENTS_NVS_NAMESPACE "Events"

/**
 * @brief Chave usada no NVS para o contador de eventos
 */
#define EVENTS_NVS_KEY "EventCounter"

/* ========================================================================
 * ESTRUTURAS DE DADOS
 * ======================================================================== */

/**
 * @brief Estrutura que representa um objeto de evento
 * 
 * Armazena o estado atual e anterior de um evento analógico,
 * permitindo detectar transições (início/fim de eventos).
 */
typedef struct {
    int state;           /**< Estado atual do evento (0=inativo, 1=ativo) */
    int previous_state;  /**< Estado anterior do evento */
} event_object;

/* ========================================================================
 * FUNÇÕES DE INICIALIZAÇÃO
 * ======================================================================== */

/**
 * @brief Inicializa o sistema NVS para persistência de eventos
 * 
 * Esta função deve ser chamada UMA ÚNICA VEZ no app_main() antes de
 * usar qualquer outra função deste módulo que acesse a Flash.
 * 
 * @return 
 *     - ESP_OK: Sucesso
 *     - ESP_ERR_*: Erro na inicialização do NVS
 * 
 * @note Esta função é idempotente - pode ser chamada múltiplas vezes
 *       sem efeitos colaterais, mas só inicializa uma vez.
 */
esp_err_t events_nvs_init(void);

/**
 * @brief Inicializa um objeto de evento
 * 
 * Zera os estados atual e anterior do evento.
 * 
 * @param[in,out] event Ponteiro para o objeto de evento a ser inicializado
 * 
 * @note Se event for NULL, a função não faz nada
 */
void event_init(event_object *event);

/* ========================================================================
 * FUNÇÕES DE GERENCIAMENTO DE EVENTOS
 * ======================================================================== */

/**
 * @brief Define um evento como ativo (ligado)
 * 
 * Marca o evento como ativo e, se for uma nova ativação, incrementa
 * o contador global de eventos ativos.
 * 
 * @param[in,out] event Ponteiro para o objeto de evento
 * 
 * @note Se o evento já estava ativo, não incrementa o contador
 * @note Se event for NULL, a função não faz nada
 */
void set_event(event_object *event);

/**
 * @brief Limpa um evento (marca como inativo)
 * 
 * Marca o evento como inativo e, se estava ativo, decrementa o contador
 * global e inicia o período de monitoramento pós-evento.
 * 
 * @param[in,out] event Ponteiro para o objeto de evento
 * 
 * @note Se event for NULL, a função não faz nada
 */
void clear_event(event_object *event);

/* ========================================================================
 * FUNÇÕES DE VERIFICAÇÃO DE ESTADO
 * ======================================================================== */

/**
 * @brief Verifica se um evento acabou de iniciar
 * 
 * Detecta transição de inativo para ativo (0 → 1).
 * 
 * @param[in] event Ponteiro para o objeto de evento
 * @return true se o evento acabou de iniciar, false caso contrário
 * 
 * @note Útil para executar ações apenas no momento do disparo
 */
bool is_event_start(event_object *event);

/**
 * @brief Verifica se um evento acabou de terminar
 * 
 * Detecta transição de ativo para inativo (1 → 0).
 * 
 * @param[in] event Ponteiro para o objeto de evento
 * @return true se o evento acabou de terminar, false caso contrário
 * 
 * @note Útil para executar ações apenas no momento da finalização
 */
bool is_event_end(event_object *event);

/**
 * @brief Verifica se um evento está ativo no momento
 * 
 * @param[in] event Ponteiro para o objeto de evento
 * @return true se o evento está ativo, false caso contrário
 */
bool is_event_on(event_object *event);

/**
 * @brief Retorna o número de eventos analógicos atualmente ativos
 * 
 * @return Quantidade de eventos ativos simultaneamente
 * 
 * @note Útil para decidir se deve enviar dados ao servidor
 */
int has_analog_events(void);

/* ========================================================================
 * FUNÇÕES DE MONITORAMENTO PÓS-EVENTO
 * ======================================================================== */

/**
 * @brief Gerencia o contador de monitoramento pós-evento
 * 
 * Controla quantos ciclos de monitoramento ainda restam após
 * o término dos eventos analógicos.
 * 
 * @param[in] check_and_decrement 
 *     - true: Decrementa o contador e retorna o valor ANTES do decremento
 *     - false: Apenas consulta o valor sem decrementar
 * 
 * @return 
 *     - Se check_and_decrement=true: Valor do contador antes do decremento (0 se já zerado)
 *     - Se check_and_decrement=false: Valor atual do contador
 * 
 * @note Use true em loops de monitoramento para consumir o contador
 * @note Use false para apenas verificar se ainda há monitoramento ativo
 */
int has_after_event_monitoring(bool check_and_decrement);

/* ========================================================================
 * FUNÇÕES DE PERSISTÊNCIA (NVS)
 * ======================================================================== */

/**
 * @brief Carrega o contador de eventos da memória Flash (NVS)
 * 
 * Lê o valor salvo do contador de monitoramento pós-evento
 * e atualiza a variável interna.
 * 
 * @return 
 *     - Valor do contador se leitura bem-sucedida
 *     - -1 em caso de erro
 * 
 * @note Deve ser chamado após events_nvs_init()
 * @note Útil para continuar monitoramento após reinicialização
 */
int get_after_event_counter(void);

/**
 * @brief Salva o contador de eventos na memória Flash (NVS)
 * 
 * Persiste o valor atual do contador de monitoramento pós-evento
 * para sobreviver a reinicializações.
 * 
 * @return 
 *     - 0: Sucesso
 *     - 1: Erro ao acessar NVS
 *     - -1: Valor do contador inválido (maior que MEASUREMENT_AFTER_ANALOG_EVENT)
 * 
 * @note Deve ser chamado após events_nvs_init()
 * @note Só salva se o valor for válido (≤ MEASUREMENT_AFTER_ANALOG_EVENT)
 */
int set_after_event_counter(void);

/* ========================================================================
 * FUNÇÕES AUXILIARES (INTERNAS)
 * ======================================================================== */

/**
 * @brief Decrementa o contador global de eventos ativos
 * 
 * @note Esta é uma função interna, normalmente não precisa ser chamada diretamente
 * @note Apenas decrementa se o contador for maior que zero
 */
void decrease_flag_counter(void);



#endif /* EVENTS_H */