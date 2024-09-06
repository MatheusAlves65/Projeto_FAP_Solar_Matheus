#include "ADC_config.h"


void adc_config() {
    // Configurar o ADC
    adc1_config_width(ADC_WIDTH_BIT_12); // Configurar a resolução para 12 bits
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // Configurar a atenuação para 11 dB
}

void adc_read() {
    // Ler o valor do ADC
    int adc_value = adc1_get_raw(ADC1_CHANNEL_0);
    // Faça algo com o valor lido...
}
