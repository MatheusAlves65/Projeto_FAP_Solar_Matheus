#ifndef Temp_H
#define Temp_H

#define TerceiraCamada_Length        200 // 10 min
#define QuartaCamada_Length          12  // 120 min
#define QuintaCamada_Length          1 // 2 horas
typedef struct {
    float media;
    float TerceiraCamada[TerceiraCamada_Length];
    float QuartaCamada[QuartaCamada_Length];
    float QuintaCamada[QuintaCamada_Length];
    uint8_t index__Terceiro;
    uint8_t index__quarto;
    uint8_t index__Quinto;
    const char* tipo;
}Temp_ADC ;

float calcular_media_temp(const float *vetor, int tamanho);

void adc_init_temp(void);

void ler_temperatura_adc(Temp_ADC *p);

#endif