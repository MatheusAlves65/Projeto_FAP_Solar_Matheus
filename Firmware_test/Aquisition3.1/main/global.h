#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>
#include "libraries.h"

//bool buffer_completo;

Energy_ADC Voltage = {.adc_channel = ADC1_CHANNEL_2, .tipo = "Tensao", .max_threshold = 100, .min_threshold = 50 };
Energy_ADC Current = {.adc_channel = ADC1_CHANNEL_3, .tipo = "Corrente",.max_threshold = 100, .min_threshold = 50};
Temp_ADC Temperatura = {.tipo = "Temperatura", .max_threshold = 100};
float calcular_media(const float *vetor, int tamanho);

event_object overVoltageEvent;
event_object overCurrentEvent;
event_object overTempEvent;
event_object underVoltageEvent; 
volatile bool buffer_completo; // Declaração da variável global


#endif // GLOBAL_H