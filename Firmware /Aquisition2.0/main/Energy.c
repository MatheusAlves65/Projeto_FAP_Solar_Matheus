#include "libraries.h"
#include "global.h"
#include "esp_log.h" // Biblioteca para ESP_LOG
#include "esp_log_color.h" // Biblioteca para ESP_LOG

#include <stdbool.h> // Para o tipo bool

int adc_value_2;
int adc_value_3;
extern volatile bool buffer_completo; // Use a variável global

void adc_setup(void) {
    // Inicializar o ADC
    adc1_channel_t channel2 = ADC1_CHANNEL_2;
    adc1_channel_t channel3 = ADC1_CHANNEL_3;
    adc1_channel_t channel4 = ADC1_CHANNEL_4; // Adicione esta linha!
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configurar os canais ADC 2, 3 e 4, atenuação
    adc1_config_channel_atten(channel2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel4, ADC_ATTEN_DB_0); // E esta!
}

void Instant_Acquisition(Energy_ADC *p) {
    adc_value_2 = adc1_get_raw(p->adc_channel);

    p->instant_value = ((float)adc_value_2 / ADC_Max_value) * V_REF;

    // Armazena os valores no buffer do primeiro nível
    p->PrimeiraCamada[p->index_primeiro] = p->instant_value;

    // Incrementa os índices e faz o wrap-around
    p->index_primeiro = (p->index_primeiro + 1) % PrimeiraCamada_Length;

    // Verifica se o buffer do primeiro nível está completo
    if (p->index_primeiro == 0) {
        p->media = calcular_media(p->PrimeiraCamada, PrimeiraCamada_Length);
        ESP_LOGI("BUFFER1", "[%s] Buffer do primeiro nível completo. Média: %.3f", p->tipo, p->media);
        buffer_completo = true; // <-- Adicione esta linha
    }
}

void After_Acquisition(Energy_ADC *p) {

    // Armazena as médias do primeiro nível no segundo nível
    p->SegundaCamada[p->index_segundo] = p->media;

    // Incrementa os índices do segundo nível
    p->index_segundo = (p->index_segundo + 1) % SecondLevel_Length;
    ESP_LOGI("BUFFER2", "Novo índice do segundo nível: %d", p->index_segundo);

    // Verifica se o buffer do segundo nível está completo
    if (p->index_segundo == 0) {

        // ---- Terceiro nivel --------
        p->TerceiraCamada[p->index__Terceiro] = calcular_media(p->SegundaCamada, SecondLevel_Length);

        // Incrementa os índices do terceiro nível
        p->index__Terceiro = (p->index__Terceiro + 1) % TerceiraCamada_Length;
        ESP_LOGI("BUFFER2", "[%s] Buffer do segundo nível completo. Média: %.3f", p->tipo, calcular_media(p->SegundaCamada, SecondLevel_Length));
    }

    if (p->index__Terceiro == 0) {
            ESP_LOGI("BUFFER3", "Buffer do terceiro nível completo.");
                // Calculo da média do terceiro nivel e armazenando no index do quarto
            p->QuartaCamada[p->index__quarto] = calcular_media(p->TerceiraCamada, TerceiraCamada_Length);
                // Incrementa os indices do Quarto nivel
            p->index__quarto = (p->index__quarto + 1) % QuartaCamada_Length;
                // Log de Debug
            ESP_LOGI("BUFFER2", "[%s] Buffer do terciro nível completo. Média: %.3f", p->tipo, calcular_media(p->TerceiraCamada, TerceiraCamada_Length));
        }

    if (p->index__quarto == 0)    {
        ESP_LOGI("BUFFER3", "Buffer do terceiro nível completo.");
        // Calculo da média do terceiro nivel e armazenando no index do quarto
        p->QuintaCamada[p->index__Quinto] = calcular_media(p->QuartaCamada, QuartaCamada_Length);
        // Incrementa os indices do Quarto nivel
        p->index__Quinto = (p->index__Quinto + 1) % QuintaCamada_Length;
        ESP_LOGI("BUFFER2", "[%s] Buffer do Quarto nível completo. Média: %.3f", p->tipo, calcular_media(p->QuartaCamada, QuartaCamada_Length));
    }

}

float calcular_media(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}