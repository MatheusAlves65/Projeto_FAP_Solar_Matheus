#include  "libraries.h"



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
    //adc_init_temp();


}


