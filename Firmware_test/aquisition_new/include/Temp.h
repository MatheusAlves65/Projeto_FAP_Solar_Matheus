#ifndef Temp_H
#define Temp_H

#define TerceiraCamada_temp_Length        200 // 10 min
#define QuartaCamada_Length          12  // 120 min
#define QuintaCamada_Length          1 // 2 horas
typedef struct {
    float real_value;
    float TerceiraCamada[TerceiraCamada_temp_Length];
    float QuartaCamada[QuartaCamada_Length];
    float QuintaCamada[QuintaCamada_Length];
    uint8_t index__Terceiro;
    uint8_t index__quarto;
    uint8_t index__Quinto;
    const char* tipo;
    float max_threshold;
    float min_threshold;  
    float offset;
    float ganho;
    bool state; // true = normal, false = em evento
}Temp_ADC ;

/**
 * @brief Calcula a média dos valores em um vetor
 * @param vetor Ponteiro para o vetor de valores
 */
float calcular_media_temp(const float *vetor, int tamanho);
/**
 * @brief Inicializa o ADC para leituras de temperatura
 * 
 */
void adc_init_temp(void);
/**
 * @brief Realiza a leitura da temperatura via ADC e atualiza os buffers de média
 * @param p Ponteiro para a estrutura Temp_ADC que contém os dados e estados do ADC
 */
void ler_temperatura_adc(Temp_ADC *p);

#endif