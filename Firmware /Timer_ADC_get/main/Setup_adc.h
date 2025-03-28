#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H
#include <driver/adc.h>
#define ADC_Max_value 4095
#define Max_Value_Voltage 190.7

// Define ADC1_CHANNEL_2 if not defined
#define ADC1_CHANNEL_2 ADC_CHANNEL_2 // tensao
#define ADC1_CHANNEL_3 ADC_CHANNEL_3 // corrente
#define ADC1_CHANNEL_4 ADC_CHANNEL_4 // temperatura

#define V_REF 1100.0 // Referência do ADC em mV (ajuste conforme necessário)



typedef struct {
    float tensao_medida;
    float corrente_medida;
} Medicoes_ADC;

// Variável global para armazenar a última medição
extern Medicoes_ADC medidas;

void adc_setup(void);

void adc_init_Reading();


void adc_init_temp (void);



#endif // 
