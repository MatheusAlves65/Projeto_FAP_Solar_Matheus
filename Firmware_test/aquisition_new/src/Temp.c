//#include "libraries.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> 
#include "driver/adc.h"
#include "esp_timer.h"
#include <esp_adc/adc_oneshot.h>
#include "esp_adc/adc_cali.h"
#include "Temp.h"
#include "Monitoring.h"


float temp_value;
int adc_value_temp;
static int cont_int = 0;
// Removed redeclaration of ADC_CHANNEL_4; use the enum constant directly

// Handle global para o ADC one-shot
extern adc_oneshot_unit_handle_t adc1_handle;
extern adc_cali_handle_t adc_cali_handle; // Referência, não definição



void ler_temperatura_adc(Temp_ADC *p) {

    cont_int++;
    //ESP_LOGI("Temperatura", " interações feitas: %d\n",cont_int);
    if (cont_int >= 15) {
    adc_oneshot_read(adc1_handle, ADC1_CHANNEL_4, &adc_value_temp);// leitura crua do adc
    int voltage_temp = 0;
    adc_cali_raw_to_voltage(adc_cali_handle, adc_value_temp, &voltage_temp);
    temp_value = voltage_temp/1000.0f; // Converte mV para Volts;
    temp_value = (temp_value * p->ganho) + p->offset; // Aplica ganho e offset
    p->real_value = temp_value;
        //ESP_LOGI("Temperatura", "16 interações feitas (~3s)\n");
        p->TerceiraCamada[p->index__Terceiro] = temp_value;
        p-> index__Terceiro++;
        if(p->real_value > p->max_threshold){
            p->state = false; // Estado de evento
        } else {
            p->state = true; // Estado normal
        }
        //ESP_LOGI("Temperatura", "Feita a leitura:%.3f\n", calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length));
        cont_int = 0;
    }

    if (p->index__Terceiro == TerceiraCamada_temp_Length) {
            //ESP_LOGI("Temperatura", "Buffer do terceiro nível completo.\n");
        p->QuartaCamada[p->index__quarto] = calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length);
        p->index__quarto++;
        p-> index__Terceiro = 0;        
            //ESP_LOGI("Temperatura", "[%s] Buffer do terciro nível completo. Média: %.3f\n", p->tipo, calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length));
    }

    if (p->index__quarto == QuartaCamada_Length) {
        //ESP_LOGI("Temperatura", "Buffer do terceiro nível completo.");
        p->QuintaCamada[p->index__Quinto] = calcular_media_temp(p->QuartaCamada, QuartaCamada_Length);
        //ESP_LOGI("Temperatura", "[%s] Buffer do Quarto nível completo. Média: %.3f\n", p->tipo, calcular_media_temp(p->QuartaCamada, QuartaCamada_Length));
        p->index__Quinto++;
        p->index__quarto = 0; 
    }
}

float calcular_media_temp(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}



