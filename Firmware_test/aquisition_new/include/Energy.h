#ifndef ENERGY_H
#define ENERGY_H
#define ADC_Max_value 4095
#define Max_Value_Voltage 190.7
#include "Events.h"
#include <driver/adc.h>
// #include <driver/adc_types.h>

#define V_REF 3300.0 // Referência do ADC em mV (ajuste conforme necessário)





#define PrimeiraCamada_Length        192 // 0.2s
#define PrimeiraCamada_past_Length  192// 0.2s
#define SecondLevel_Length           16 // 3s
#define TerceiraCamada_Length        200 // 10 min
#define QuartaCamada_Length          12  // 120 min
#define QuintaCamada_Length          1 // 2 horas

/**
 * @brief Estrutura para armazenar os dados e estados do ADC de energia
 * Inclui buffers para múltiplos níveis de média, índices para controle dos buffers, eventos de limite e estado geral.
 * @param vetor Buffer de valores para cálculo de média
 * @param tamanho Tamanho do buffer para cálculo de média
 */
float calcular_media(const float *vetor, int tamanho);

/**
 * @brief Estrutura principal para gerenciamento do ADC de energia, incluindo buffers para múltiplos níveis de média, índices de controle, eventos de limite e estado geral.
 * @param after Indica se a aquisição foi realizada (true) ou se está aguardando (false)
 * @param instant_value Valor instantâneo lido do ADC
 * @param media Média calculada a partir do buffer de valores
 * @param PrimeiraCamada Buffer para armazenar valores do primeiro nível de média (0.2s)
 * @param SegundaCamada Buffer para armazenar valores do segundo nível de média (3s
 * @param TerceiraCamada Buffer para armazenar valores do terceiro nível de média (10 min)
 * @param QuartaCamada Buffer para armazenar valores do quarto nível de média (120 min)
 * @param QuintaCamada Buffer para armazenar valores do quinto nível de média (2 horas)
 * @param index_primeiro Índice para controle do buffer do primeiro nível
 * @param index_segundo Índice para controle do buffer do segundo nível
 * @param index__Terceiro Índice para controle do buffer do terceiro nível
 * @param index__quarto Índice para controle do buffer do quarto nível
 * @param index__Quinto Índice para controle do buffer do quinto nível
 * @param adc_channel Canal do ADC utilizado para leitura
 * @param tipo Tipo de energia monitorada (ex: "Tensão", "Corrente")
 * @param primeiro_nivel_ciclos Número de ciclos para completar o buffer do primeiro nível
 * @param max_threshold Limite superior para detecção de evento
 * @param min_threshold Limite inferior para detecção de evento
 * @param state Estado atual do monitoramento (true=normal, false=em evento)
 */
typedef struct {

    bool after;

    float instant_value;

    float media;
        
    float PrimeiraCamada[PrimeiraCamada_Length];
    float PrimeiraCamada_past[PrimeiraCamada_past_Length];
    float SegundaCamada[SecondLevel_Length];
    float TerceiraCamada[TerceiraCamada_Length];
    float QuartaCamada[QuartaCamada_Length];
    float QuintaCamada[QuintaCamada_Length];
        
    uint8_t index_primeiro;
    uint8_t index_segundo;
    uint8_t index__Terceiro;
    uint8_t index__quarto;
    uint8_t index__Quinto;
    adc1_channel_t adc_channel;
    const char* tipo;
    uint32_t primeiro_nivel_ciclos; 
    float max_threshold;                      
    float min_threshold;
    bool state;
    float Limiar_sup;
    float Limiar_inf;
    float valor_3s;
    float offset;
    float ganho;
    uint32_t tempo_confirmacao_ms;
    uint32_t contador_amostras;
    uint32_t MOSFET_active_ms;
    uint32_t tempo_acionamento_mosfet_ms;
} Energy_ADC;

/**
 * @brief Configura o ADC para leituras de energia
 */
void adc_setup(void);

/**
 * @brief Realiza a aquisição instantânea do ADC e atualiza os buffers do primeiro nível
 * @param p Ponteiro para a estrutura Energy_ADC que contém os dados e estados do ADC de energia
 */
void Instant_Acquisition(Energy_ADC *p);

/**
 * @brief Processa os dados após a aquisição, atualizando os buffers dos níveis superiores e gerenciando eventos de limite
 * @param p Ponteiro para a estrutura Energy_ADC que contém os dados e estados do ADC de energia
 */
void After_Acquisition(Energy_ADC *p);
/**
 * @brief Inicializa o ADC de temperatura
 */
void adc_init_temp (void);
/**
 * @brief Inicializa a estrutura Energy_ADC com valores padrão
 * @param p Ponteiro para a estrutura Energy_ADC a ser inicializada
 */
void Energy_ADC_Init(Energy_ADC *p);





#endif // 
