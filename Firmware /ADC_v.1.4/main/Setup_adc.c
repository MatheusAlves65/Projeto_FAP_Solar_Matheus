#include "Setup_adc.h"
#include <driver/adc.h>
#include "esp_adc_cal.h" // Include the header for ADC calibration and channel definitions
#include "driver/adc_types_legacy.h"
#include "driver/adc.h" // Include the header for ADC channel definitions
#include "driver/adc.h" // Include the header for ADC channel definitions

int adc_value_2;
int adc_value_3;
int adc_value_1;
float tensao;

void adc_setup(void) {
    // Inicializar o ADC
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configurar os canais ADC 2, 3 e 4, atenuaçao
    adc1_config_channel_atten(ADC1_CHANNEL_2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_0);
}

float adc_init_voltage(float tensao) { // Configurando a resolução do ADC para a leitura de voltagem
    adc_value_2 = adc1_get_raw(ADC1_CHANNEL_2);
    float tensao_medida = (adc_value_2 / (float)ADC_Max_value) * Max_Value_Voltage;
    tensao = tensao_medida;
    //printf("Tensão Medida: %.2f\n", tensao_medida);
    return tensao;
}

float corrent_medida;

void adc_init_current (void) { // Configurando a resolução do ADC para leitura da corrente
    adc_value_3 = adc1_get_raw(ADC1_CHANNEL_3);
    corrent_medida = (adc_value_3 / (float)ADC_Max_value) * Max_Value_Voltage; // arrumar!!!!
    printf("Corrente Medida: %.2f\n", corrent_medida);
}

void adc_init_temp (void) {  // Configurando a resolução do ADC para leitura da temperatura
     adc_value_1 = adc1_get_raw(ADC1_CHANNEL_1);
    float temp_medida = (adc_value_1 / (float)ADC_Max_value) * Max_Value_Voltage; // arrumar!!!!
    printf("Temperatura Medida: %.2f\n", temp_medida);
}