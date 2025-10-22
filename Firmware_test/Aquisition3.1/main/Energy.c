#include "libraries.h"
#include "global.h"
#include "esp_log.h"
#include "esp_log_color.h"
#include <stdbool.h>
#include <driver/adc.h>


int adc_value_2;
extern volatile bool buffer_completo;

// Handle global para o ADC one-shot
adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t adc_cali_handle = NULL; // Handle global para calibração
event_object overVoltageEvent;
event_object overCurrentEvent;
event_object overTempEvent;
event_object underVoltageEvent; 

void Energy_ADC_Init(Energy_ADC *p) {
    // Zera índices
    p->index_primeiro = 0;  // Indice da primeira camada
    p->index_segundo = 0;   // Indice da segunda camada
    p->index__Terceiro = 0; // Indice da terceira camada
    p->index__quarto = 0;   // Indice da quarta camada
    p->index__Quinto = 0;   // Indice da quinta camada

    // Zera buffers
    for (int i = 0; i < PrimeiraCamada_Length; i++) {
        p->PrimeiraCamada[i] = 0.0f;
    }
    for (int i = 0; i < SecondLevel_Length; i++) {
        p->SegundaCamada[i] = 0.0f;
    }
    for (int i = 0; i < TerceiraCamada_Length; i++) {
        p->TerceiraCamada[i] = 0.0f;
    }
    for (int i = 0; i < QuartaCamada_Length; i++) {
        p->QuartaCamada[i] = 0.0f;
    }
    for (int i = 0; i < QuintaCamada_Length; i++) {
        p->QuintaCamada[i] = 0.0f;
    }

    p->instant_value = 0.0f;
    p->media = 0.0f;
}

void adc_setup(void) {
    // Configuração do ADC em modo one-shot
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1, // qual barramento do adc utlizado, nesse caso o 1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Modo ultra low power desabilitado
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    // Configurar canais
    adc_oneshot_chan_cfg_t chan_cfg = { // configuração do ADC
        .bitwidth = ADC_BITWIDTH_12, // resolução do adc
        .atten = ADC_ATTEN_DB_12, // atenuação por ser 3.3V
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_2, &chan_cfg); // Corrente
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3, &chan_cfg); // Tensão
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &chan_cfg); // Temperatura
        led_rgb_set_color(0, 0, 255);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        led_rgb_off();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Inicializa calibração
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle); // cria a curva de calibração
}

void Instant_Acquisition(Energy_ADC *p) {
    // Leitura one-shot do ADC
    adc_oneshot_read(adc1_handle, p->adc_channel, &adc_value_2); // leitura crua do adc

    // Sempre usa calibração para converter para mV e depois para Volts
    int voltage_mv = 0;
    adc_cali_raw_to_voltage(adc_cali_handle, adc_value_2, &voltage_mv);
    p->instant_value = voltage_mv ; 

    // Armazena o valor instantâneo no buffer do primeiro nivel
    p->PrimeiraCamada[p->index_primeiro] = p->instant_value;

    // Atualiza o índice do buffer circular do primeiro nivel
    p->index_primeiro = (p->index_primeiro + 1) % PrimeiraCamada_Length;

    // Se o buffer do primeiro nivel está cheio, calcula a média e sinaliza
    if (p->index_primeiro == 0) {
        p->media = calcular_media(p->PrimeiraCamada, PrimeiraCamada_Length);
        ESP_LOGI("BUFFER1", "[%s] Buffer do primeiro nivel completo. Média: %.3f\n", p->tipo, p->media);
        buffer_completo = true;
    }
}

void After_Acquisition(Energy_ADC *p) {
    // Armazena as médias do primeiro nivel no segundo nivel
    p->SegundaCamada[p->index_segundo] = p->media;
   ESP_LOGI("BUFFER2", "[%s]Novo índice do segundo nivel: %d\n",p->tipo, p->index_segundo);

    p->index_segundo++;

    // Verifica se o buffer do segundo nivel está completo
    if (p->index_segundo == SecondLevel_Length) {
        ESP_LOGI("BUFFER2", "[%s]Buffer do segundo nivel completo.\n",p->tipo);

        // ---- Terceiro nivel --------

        p->TerceiraCamada[p->index__Terceiro] = calcular_media(p->SegundaCamada, SecondLevel_Length);


        p-> index__Terceiro++;

        p-> index_segundo = 0;

    }
   ESP_LOGI("BUFFER3", "[%s]Novo índice do terceiro nivel: %d\n",p->tipo, p->index__Terceiro);
        // Inicio da medição do Terceiro niv

    if (p->index__Terceiro == TerceiraCamada_Length) {
        ESP_LOGI("BUFFER3", "[%s]Buffer do terceiro nivel completo.\n", p->tipo);
       ESP_LOGI("BUFFER3", "[%s] Buffer do terciro nivel completo. Média: %.3f\n", p->tipo, calcular_media(p->TerceiraCamada, TerceiraCamada_Length));
        // Quarto nivel
            p->QuartaCamada[p->index__quarto] = calcular_media(p->TerceiraCamada, TerceiraCamada_Length);
        // Incrementa os indices do Quarto nivel

        p->index__quarto++;

        p-> index__Terceiro = 0;


        }
   ESP_LOGI("BUFFER4", "[%s]Novo índice do Quarto nivel: %d\n", p->tipo, p->index__quarto);
    if (p->index__quarto == QuartaCamada_Length)    {

        ESP_LOGI("BUFFER4", "[%s]Buffer do quarto nivel completo.\n", p->tipo);
        // Calculo da média do terceiro nivel e armazenando no index do quarto
        p->QuintaCamada[p->index__Quinto] = calcular_media(p->QuartaCamada, QuartaCamada_Length);

        ESP_LOGI("BUFFER4", "[%s] Buffer do Quarto nivel completo. Média: %.3f\n", p->tipo, calcular_media(p->QuartaCamada, QuartaCamada_Length));

        p->index__Quinto++;
        p->index__quarto = 0;    
    }

}

float calcular_media(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}