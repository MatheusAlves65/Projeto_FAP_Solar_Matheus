#ifndef Temp_H
#define Temp_H

#define SecondLevel_Length           15
#define TerceiraCamada_Length        10
typedef struct {
    float media;
    float SegundaCamada[SecondLevel_Length];    
    float TerceiraCamada[TerceiraCamada_Length];
    uint8_t index_segundo;    
    uint8_t index__Terceiro;
}Temp_ADC ;


void adc_init_temp(void);



#endif