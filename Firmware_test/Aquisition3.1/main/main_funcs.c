#include  "libraries.h"

Energy_ADC Voltage = {.adc_channel = ADC1_CHANNEL_2, .tipo = "Tensao" };
Energy_ADC Current = {.adc_channel = ADC1_CHANNEL_3, .tipo = "Corrente"};
Temp_ADC Temperatura = {.tipo = "Temperatura"};

void Instantaneous_Energy(){ // primeiro nivel do buffer

    Instant_Acquisition(&Voltage);
    Instant_Acquisition(&Current);
}

void Continuous_Energy(){
    After_Acquisition(&Voltage);
    After_Acquisition(&Current);
    ler_temperatura_adc(&Temperatura);

}

void Initialize_Parameters(){
    Energy_ADC_Init(&Voltage);
    Energy_ADC_Init(&Current);
    //Energy_ADC_Init(&Temperatura);


}


