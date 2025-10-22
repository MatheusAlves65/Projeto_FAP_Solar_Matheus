#include "global.h"
#include <stdbool.h>

volatile bool buffer_completo = false; // Definição da variável global
 Energy_ADC Voltage = {.adc_channel = ADC1_CHANNEL_2, .tipo = "Tensao", .max_threshold = 100, .min_threshold = 50 };
 Energy_ADC Current = {.adc_channel = ADC1_CHANNEL_3, .tipo = "Corrente",.max_threshold = 100, .min_threshold = 50};
 Temp_ADC Temperatura = {.tipo = "Temperatura", .max_threshold = 100};


// Função para calcular media

