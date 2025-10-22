#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>
#include "libraries.h"

//bool buffer_completo;

extern Energy_ADC Voltage;
extern Energy_ADC Current;
extern Temp_ADC Temperatura;
extern float calcular_media(const float *vetor, int tamanho);


extern volatile bool buffer_completo; // Declaração da variável global


#endif // GLOBAL_H