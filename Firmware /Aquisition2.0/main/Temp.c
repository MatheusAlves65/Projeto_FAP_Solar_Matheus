#include "libraries.h"
#include "driver/adc.h"

adc1_channel_t channel4 = ADC1_CHANNEL_4;
float ler_temperatura_adc() {
    int adc_value = adc1_get_raw(ADC1_CHANNEL_4);
    return ((float)adc_value / (float)ADC_Max_value) * V_REF;
}


