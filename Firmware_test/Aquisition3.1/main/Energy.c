#include "libraries.h"
#include "global.h"
#include "esp_log.h" // Biblioteca para ESP_LOG
#include "esp_log_color.h" // Biblioteca para ESP_LOG

#include <stdbool.h> // Para o tipo bool

int adc_value_2;
int adc_value_3;
extern volatile bool buffer_completo; // Use a variável global



void Energy_ADC_Init(Energy_ADC *p) {
    // Zera índices
    p->index_primeiro = 0;
    p->index_segundo = 0;
    p->index__Terceiro = 0;
    p->index__quarto = 0;
    p->index__Quinto = 0;

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

    p->instant_value = ((float)adc_value_2 / ADC_Max_value) * V_REF; // depois jogar o ganho de cada sensor!!!!!!!

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
    ESP_LOGI("BUFFER2", "Novo índice do segundo nível: %d", p->index_segundo);

    p->index_segundo++;

    // Verifica se o buffer do segundo nível está completo
    if (p->index_segundo == SecondLevel_Length) {
        /*tempo_fim_segundo = esp_timer_get_time();
        int64_t tempo_total_segundo = tempo_fim_segundo - tempo_inicio_segundo;
        float tempo_total_segundo_s = tempo_total_segundo / 1000000.0f;
        ESP_LOGI("BUFFER2", "[%s] Tempo para preencher todo o buffer do segundo nível: %.2f s", p->tipo, tempo_total_segundo_s);*/
        ESP_LOGI("BUFFER2", "Buffer do segundo nível completo.");

        // ---- Terceiro nivel --------

        p->TerceiraCamada[p->index__Terceiro] = calcular_media(p->SegundaCamada, SecondLevel_Length);

        //p->index__Terceiro = (p->index__Terceiro + 1) % TerceiraCamada_Length;

        p-> index__Terceiro++;

        p-> index_segundo = 0;

    }

        // Inicio da medição do Terceiro niv

    ESP_LOGI("BUFFER3", "Novo índice do terceiro nível: %d", p->index__Terceiro);
    if (p->index__Terceiro == TerceiraCamada_Length) {
        //ESP_LOGI("BUFFER2", "Buffer do segundo nível completo.");
        ESP_LOGI("BUFFER3", "Buffer do terceiro nível completo.");
        ESP_LOGI("BUFFER3", "[%s] Buffer do terciro nível completo. Média: %.3f", p->tipo, calcular_media(p->TerceiraCamada, TerceiraCamada_Length));
        // Quarto nivel
            p->QuartaCamada[p->index__quarto] = calcular_media(p->TerceiraCamada, TerceiraCamada_Length);
        // Incrementa os indices do Quarto nivel

        p->index__quarto++;

        p-> index__Terceiro = 0;


        }
    ESP_LOGI("BUFFER4", "Novo índice do Quarto nível: %d", p->index__quarto);
    if (p->index__quarto == QuartaCamada_Length)    {

        ESP_LOGI("BUFFER4", "Buffer do quarto nível completo.");
        // Calculo da média do terceiro nivel e armazenando no index do quarto
        p->QuintaCamada[p->index__Quinto] = calcular_media(p->QuartaCamada, QuartaCamada_Length);

        ESP_LOGI("BUFFER4", "[%s] Buffer do Quarto nível completo. Média: %.3f", p->tipo, calcular_media(p->QuartaCamada, QuartaCamada_Length));

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