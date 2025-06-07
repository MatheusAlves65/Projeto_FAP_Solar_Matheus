#include "libraries.h"
#include "driver/adc.h"
#include "esp_timer.h"

adc1_channel_t channel4 = ADC1_CHANNEL_4;
void ler_temperatura_adc(Temp_ADC *p) {
    static int cont_int = 0;
    static int64_t ultimo_tempo_leitura = 0; // Tempo da última leitura válida

    int adc_value = adc1_get_raw(ADC1_CHANNEL_4);
    float temp_value = ((float)adc_value / ADC_Max_value);
    cont_int++;
    if (cont_int >= 15) {
        int64_t tempo_atual = esp_timer_get_time();
        if (ultimo_tempo_leitura != 0) {
            int64_t delta_us = tempo_atual - ultimo_tempo_leitura;
            float delta_s = delta_us / 1000000.0;
            ESP_LOGI("Temperatura", "Tempo entre leituras de temperatura: %.2f s", delta_s);
        }
        ultimo_tempo_leitura = tempo_atual;

        ESP_LOGI("Temperatura", "15 interações feitas (~3s)");
        p->TerceiraCamada[p->index__Terceiro] = temp_value;
        p->index__Terceiro = (p->index__Terceiro + 1) % TerceiraCamada_temp_Length;
        ESP_LOGI("Temperatura", "Feita a leitura:%.3f", calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length));
        cont_int = 0;
    }

    if (p->index__Terceiro == 0) {
        ESP_LOGI("Temperatura", "Buffer do terceiro nível completo.");
        p->QuartaCamada[p->index__quarto] = calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length);
        p->index__quarto = (p->index__quarto + 1) % QuartaCamada_Length;
        ESP_LOGI("Temperatura", "[%s] Buffer do terciro nível completo. Média: %.3f", p->tipo, calcular_media_temp(p->TerceiraCamada, TerceiraCamada_temp_Length));
    }

    if (p->index__quarto == 0) {
        ESP_LOGI("Temperatura", "Buffer do terceiro nível completo.");
        p->QuintaCamada[p->index__Quinto] = calcular_media_temp(p->QuartaCamada, QuartaCamada_Length);
        p->index__Quinto = (p->index__Quinto + 1) % QuintaCamada_Length;
        ESP_LOGI("Temperatura", "[%s] Buffer do Quarto nível completo. Média: %.3f", p->tipo, calcular_media_temp(p->QuartaCamada, QuartaCamada_Length));
    }
}

float calcular_media_temp(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}



