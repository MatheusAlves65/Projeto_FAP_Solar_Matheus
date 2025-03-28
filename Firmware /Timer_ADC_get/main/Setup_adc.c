#include "libraries.h"



int adc_value_2;
int adc_value_3;
int adc_value_1;
void adc_setup(void) {
    // Inicializar o ADC
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configurar os canais ADC 2, 3 e 4, atenuaçao
    adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_0);
}

void adc_init_Reading() { // Configurando a resolução do ADC para a leitura de voltagem
    adc_value_2 = adc1_get_raw(ADC1_CHANNEL_2);
    adc_value_3 = adc1_get_raw(ADC1_CHANNEL_3);
    medidas.tensao_medida = ((float)adc_value_2 /ADC_Max_value) * V_REF ;
    medidas.corrente_medida = ((float)adc_value_3 /ADC_Max_value) * V_REF ;
}





void adc_init_temp (void) {  // Configurando a resolução do ADC para leitura da temperatura
     adc_value_1 = adc1_get_raw(ADC1_CHANNEL_4);
    float temp_medida = (adc_value_1 / (float)ADC_Max_value) * Max_Value_Voltage; // arrumar!!!!
    printf("Temperatura Medida: %.2f\n", temp_medida);
}