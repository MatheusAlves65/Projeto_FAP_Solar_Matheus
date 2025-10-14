#include "libraries.h"
#include "driver/adc.h"
#include "esp_timer.h"


float temp_value;
int adc_value_temp;
static int cont_int = 0;
// Removed redeclaration of ADC_CHANNEL_4; use the enum constant directly

// Handle global para o ADC one-shot
extern adc_oneshot_unit_handle_t adc1_handle;
extern adc_cali_handle_t adc_cali_handle; // Referência, não definição


/*void adc_init_temp(void) {
    // Configuração do ADC em modo one-shot
    adc_oneshot_unit_init_cfg_t init_config_temp = {
        .unit_id = ADC_UNIT_1, // qual barramento do adc utlizado, nesse caso o 1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Modo ultra low power desabilitado
    };
    adc_oneshot_new_unit(&init_config_temp, &adc1_handle);

    // Configurar canais
    adc_oneshot_chan_cfg_t chan_cfg_temp = { // configuração do ADC
        .bitwidth = ADC_BITWIDTH_12, // resolução do adc
        .atten = ADC_ATTEN_DB_12, // atenuação por ser 3.3V
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &chan_cfg_temp); // Temperatura

    // Inicializa calibração
    adc_cali_curve_fitting_config_t cali_config_temp = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_curve_fitting(&cali_config_temp, &adc_cali_handle); // cria a curva de calibração
}*/

void ler_temperatura_adc(Temp_ADC *p) {

    cont_int++;
    ESP_LOGI("Temperatura", " interações feitas: %d\n",cont_int);
    if (cont_int >= 15) {
    adc_oneshot_read(adc1_handle, ADC1_CHANNEL_4, &adc_value_temp);// leitura crua do adc
    int voltage_temp = 0;
    adc_cali_raw_to_voltage(adc_cali_handle, adc_value_temp, &voltage_temp);
    temp_value = voltage_temp ;
        ESP_LOGI("Temperatura", "16 interações feitas (~3s)\n");
        p->TerceiraCamada[p->index__Terceiro] = temp_value;
        p-> index__Terceiro++;
        ESP_LOGI("Temperatura", "Feita a leitura:%.3f\n", calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length));
        cont_int = 0;
    }

    if (p->index__Terceiro == TerceiraCamada_Length) {
            ESP_LOGI("Temperatura", "Buffer do terceiro nível completo.\n");
        p->QuartaCamada[p->index__quarto] = calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length);
        p->index__quarto++;
        p-> index__Terceiro = 0;        
            ESP_LOGI("Temperatura", "[%s] Buffer do terciro nível completo. Média: %.3f\n", p->tipo, calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length));
    }

    if (p->index__quarto == QuartaCamada_Length) {
        ESP_LOGI("Temperatura", "Buffer do terceiro nível completo.");
        p->QuintaCamada[p->index__Quinto] = calcular_media_temp(p->QuartaCamada, QuartaCamada_Length);
        ESP_LOGI("Temperatura", "[%s] Buffer do Quarto nível completo. Média: %.3f\n", p->tipo, calcular_media_temp(p->QuartaCamada, QuartaCamada_Length));
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



