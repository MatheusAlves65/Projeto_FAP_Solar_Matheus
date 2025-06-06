#ifndef ADC_CONFIG_H
#define ADC_CONFIG_H
#define ADC_Max_value 4095
#define Max_Value_Voltage 190.7


#define V_REF 1100.0 // Referência do ADC em mV (ajuste conforme necessário)


void adc_setup(void);

void adc_init_Reading();


void adc_init_temp (void);



#endif // 
