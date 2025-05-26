#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H
#define ADC_Max_value 4095
#define Max_Value_Voltage 190.7
#include <driver/adc.h>
// #include <driver/adc_types.h>

#define V_REF 1100.0 // Referência do ADC em mV (ajuste conforme necessário)


#define PrimeiraCamada_Length        192
#define SecondLevel_Length           15
#define TerceiraCamada_Length        10
#define QuartaCamada_Length          12
#define QuintaCamada_Length          1

float calcular_media(const float *vetor, int tamanho);

// Definindo a struct de medições do ADC
typedef struct {

    bool after;

    float instant_value;

    float media;
        
    float PrimeiraCamada[PrimeiraCamada_Length];
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
} Energy_ADC;

void adc_setup(void);

void Instant_Acquisition(Energy_ADC *p);

void After_Acquisition(Energy_ADC *p);

void adc_init_temp (void);





#endif // 
