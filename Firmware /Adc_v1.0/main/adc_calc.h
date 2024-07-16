#ifndef ADC_H
#define ADC_H

#include "driver/adc.h"


// Definições de configuração do ADC
#define ADC_CHANNELvoltage ADC1_CHANNEL_6  // Canal do ADC para tensao
#define ADC_CHANNEL35 ADC1_CHANNEL_35  // Canal do ADC para tensao
#define ADC_WIDTH ADC_WIDTH_BIT_9  // Largura do ADC de 12 bits
#define ADC_ATTEN ADC_ATTEN_DB_0    // Sem atenuação
#define V_REF 1.1                   // Tensão de referência em Volts
#define ADC_MAX 4095                // Valor máximo do ADC de 12 bits
#define TARGET_VOLTAGE 0.293        // Tensão alvo para 50V

// Funções para inicializar o ADC e converter valores
void init_adc(void);
float adc_to_voltage(int adc_value);

#endif // ADC_H
