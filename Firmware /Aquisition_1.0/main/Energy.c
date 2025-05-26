#include "libraries.h"
#include "global.h"
#include "esp_log.h" // Biblioteca para ESP_LOG
#include "esp_log_color.h" // Biblioteca para ESP_LOG

int adc_value_2;
int adc_value_3;

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
    static int64_t buffer_start_time = 0;

    // Inicializa o tempo de início quando o índice é 0
    if (p -> index_primeiro == 0) {
        buffer_start_time = esp_timer_get_time();
    }
    adc_value_2 = adc1_get_raw(p->adc_channel);
    p->instant_value = ((float)adc_value_2 / ADC_Max_value) * V_REF;

    // Armazena os valores no buffer do primeiro nível
    p->PrimeiraCamada[p->index_primeiro] = p->instant_value;

    // Incrementa os índices e faz o wrap-around
    p->index_primeiro = (p->index_primeiro + 1) % PrimeiraCamada_Length;

    // Verifica se o buffer do primeiro nível está completo
    if (p->index_primeiro == 0) {
        int64_t buffer_end_time = esp_timer_get_time();
        int64_t total_buffer_time = buffer_end_time - buffer_start_time;

        // Log do tempo do primeiro nível
        ESP_LOGI("Timing", "Tempo do primeiro nível: %lld us", total_buffer_time);

        // Para calcular a média do primeiro nível:
        p->media = calcular_media(p->PrimeiraCamada, PrimeiraCamada_Length);

        buffer_completo = true;
    }
}

void After_Acquisition(Energy_ADC *p) {
    static int64_t buffer_start_time2 = 0;

    if (p->index_segundo == 0) {
        buffer_start_time2 = esp_timer_get_time();
    }

    // Armazena as médias do primeiro nível no segundo nível
    p->SegundaCamada[p->index_segundo] = p->media;

    // --- Leitura e armazenamento da temperatura usando a struct ---

    // Incrementa os índices do segundo nível
    p->index_segundo = (p->index_segundo + 1) % SecondLevel_Length;

    // Verifica se o buffer do segundo nível está completo
    if (p->index_segundo == 0) {
        int64_t buffer_end_time2 = esp_timer_get_time();
        int64_t total_buffer_time2 = buffer_end_time2 - buffer_start_time2;

        ESP_LOGI("Timing", "Tempo do segundo nível: %lld us", total_buffer_time2);

        float media_v2 = calcular_media(p->SegundaCamada, SecondLevel_Length);


        // ---- Terceiro nivel --------


        // Armazena as médias no terceiro nível
        p->TerceiraCamada[p->index__Terceiro] = media_v2;

        // Incrementa os índices do terceiro nível
        p->index__Terceiro = (p->index__Terceiro + 1) % TerceiraCamada_Length;
        ESP_LOGI("Third_Level", "Índice do terceiro nível de %d: %d",p->tipo, p->index__Terceiro);


        // Adicione uma variável estática para armazenar o tempo de início do terceiro nível
        static int64_t buffer_start_time3 = 0;

        // Dentro da verificação do terceiro nível
        if (p->index__Terceiro == 0) {
            // Se o índice for 0, significa que o terceiro nível foi preenchido
            int64_t buffer_end_time3 = esp_timer_get_time();
            int64_t total_buffer_time3 = buffer_end_time3 - buffer_start_time3;

            // Log do tempo total do terceiro nível

            // Reinicia o tempo de início para o próximo ciclo
            buffer_start_time3 = esp_timer_get_time();

            // Calcula a média do terceiro nível
            float soma_v3 = 0.0, soma_i3 = 0.0;
            for (int i = 0; i < TerceiraCamada_Length; i++) {
                soma_v3 += p->TerceiraCamada[i];
            }
            float media_v3 = soma_v3 / TerceiraCamada_Length;

            ESP_LOGI(V_Third, "Média do terceiro nível de %d: %.3f V",p->tipo, media_v3);
        } else {
            // Inicializa o tempo de início na primeira execução
            if (buffer_start_time3 == 0) {
                buffer_start_time3 = esp_timer_get_time();
            }
        }
    }
}


float calcular_media(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}