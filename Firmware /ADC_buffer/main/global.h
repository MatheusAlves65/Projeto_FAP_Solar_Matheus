#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>
#define PrimeiraCamada_Length        192
#define SecondLevel_Length           15
#define TerceiraCamada_Length        200
#define QuartaCamada_Length          12
#define QuintaCamada_Length          1


// Definindo a struct de medições do ADC
typedef struct {
    float tensao_medida;
    float corrente_medida;
        
    float PrimeiraCamada[PrimeiraCamada_Length];
    float SegundaCamada[SecondLevel_Length];
    float TerceiraCamada[TerceiraCamada_Length];
    float QuartaCamada[QuartaCamada_Length];
    float QuintaCamada[QuintaCamada_Length];
        
    uint8_t index_primeiro;
    uint8_t index__segundo;
    uint8_t index__Terceiro;
    uint8_t index__quarto;
    uint8_t index__Quinto;

    uint32_t primeiro_nivel_ciclos; 
} Medicoes_ADC;

// Declaração externa da variável global
extern Medicoes_ADC medidas;

#endif // GLOBAL_H