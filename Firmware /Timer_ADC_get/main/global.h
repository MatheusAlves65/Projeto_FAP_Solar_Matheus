#ifndef GLOBAL_H
#define GLOBAL_H

// Definindo a struct de medições do ADC
typedef struct {
    float tensao_medida;
    float corrente_medida;
} Medicoes_ADC;

// Declaração externa da variável global
extern Medicoes_ADC medidas;

#endif // GLOBAL_H