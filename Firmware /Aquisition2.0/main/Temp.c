#include "libraries.h"
#include "driver/adc.h"

adc1_channel_t channel4 = ADC1_CHANNEL_4;
void ler_temperatura_adc(Temp_ADC *p) {
    int adc_value = adc1_get_raw(ADC1_CHANNEL_4);
    float temp_value = ((float)adc_value / ADC_Max_value);
        p->TerceiraCamada[p->index__Terceiro] = temp_value;

        p->index__Terceiro = (p->index__Terceiro + 1) % TerceiraCamada_Length;

    if (p->index__Terceiro == 0) {
            ESP_LOGI("BUFFER3", "Buffer do terceiro nível completo.");
                // Calculo da média do terceiro nivel e armazenando no index do quarto
            p->QuartaCamada[p->index__quarto] = calcular_media_temp(p->TerceiraCamada, TerceiraCamada_Length);
                // Incrementa os indices do Quarto nivel
            p->index__quarto = (p->index__quarto + 1) % QuartaCamada_Length;
                // Log de Debug
            ESP_LOGI("BUFFER2", "[%s] Buffer do terciro nível completo. Média: %.3f", p->tipo, calcular_media_temp(p->TerceiraCamada, TerceiraCamada_Length));
        }

    if (p->index__quarto == 0)    {
        ESP_LOGI("BUFFER3", "Buffer do terceiro nível completo.");
        // Calculo da média do terceiro nivel e armazenando no index do quarto
        p->QuintaCamada[p->index__Quinto] = calcular_media_temp(p->QuartaCamada, QuartaCamada_Length);
        // Incrementa os indices do Quarto nivel
        p->index__Quinto = (p->index__Quinto + 1) % QuintaCamada_Length;
        ESP_LOGI("BUFFER2", "[%s] Buffer do Quarto nível completo. Média: %.3f", p->tipo, calcular_media_temp(p->QuartaCamada, QuartaCamada_Length));
    }

}

float calcular_media_temp(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}



