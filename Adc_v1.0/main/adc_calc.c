#include "adc_calc.h"

// Função para inicializar o ADC
void init_adc(void) {
    adc1_config_width(ADC_WIDTH);
    adc1_config_channel_atten(ADC_CHANNELvoltage, ADC_ATTEN);
}

// Função para mapear valor do ADC para tensão de saída desejada
float adc_to_voltage(int adc_value) {
    // Converte o valor do ADC para tensão (em Volts)
    float input_voltage = ((float)adc_value / ADC_MAX) * V_REF;
    
    // Mapeia a tensão de entrada para a tensão de saída desejada
    float output_voltage = (input_voltage * 500) / TARGET_VOLTAGE;
    return output_voltage;
}
