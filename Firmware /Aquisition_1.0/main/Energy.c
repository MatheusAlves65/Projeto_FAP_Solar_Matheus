#include "libraries.h"
#include "global.h"

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

void Instant_Acquisition() {
    static int64_t buffer_start_time = 0;

    // Inicializa o tempo de início quando o índice é 0
    if (Tensao.index_primeiro == 0) {
        buffer_start_time = esp_timer_get_time();
    }
    adc_value_2 = adc1_get_raw(ADC1_CHANNEL_2);
    adc_value_3 = adc1_get_raw(ADC1_CHANNEL_3);
    Tensao.instant_value = ((float)adc_value_2 / ADC_Max_value) * V_REF;
    Corrente.instant_value = ((float)adc_value_3 / ADC_Max_value) * V_REF;

    // Armazena os valores no buffer do primeiro nível
    Tensao.PrimeiraCamada[Tensao.index_primeiro] = Tensao.instant_value;
    Corrente.PrimeiraCamada[Corrente.index_primeiro] = Corrente.instant_value;

    // Incrementa os índices e faz o wrap-around
    Tensao.index_primeiro = (Tensao.index_primeiro + 1) % PrimeiraCamada_Length;
    Corrente.index_primeiro = (Corrente.index_primeiro + 1) % PrimeiraCamada_Length;

    // Verifica se o buffer do primeiro nível está completo
    if (Tensao.index_primeiro == 0) {
        int64_t buffer_end_time = esp_timer_get_time();
        int64_t total_buffer_time = buffer_end_time - buffer_start_time;

        // Log do tempo do primeiro nível
        ESP_LOGI("Timing", "Tempo do primeiro nível: %lld us", total_buffer_time);

        // Para calcular a média do primeiro nível:
        Tensao.media = calcular_media(Tensao.PrimeiraCamada, PrimeiraCamada_Length);
        Corrente.media = calcular_media(Corrente.PrimeiraCamada, PrimeiraCamada_Length);

        buffer_completo = true;
    }
}

void After_Acquisition() {
    static int64_t buffer_start_time2 = 0;

    if (Tensao.index_segundo == 0) {
        buffer_start_time2 = esp_timer_get_time();
    }

    // Armazena as médias do primeiro nível no segundo nível
    Tensao.SegundaCamada[Tensao.index_segundo] = Tensao.media;
    Corrente.SegundaCamada[Corrente.index_segundo] = Corrente.media;

    // --- Leitura e armazenamento da temperatura usando a struct ---
    float temp_atual = ler_temperatura_adc();
    ESP_LOGI(Temp, "Leitura instantânea da temperatura: %.2f", temp_atual);
    Temperatura.SegundaCamada[Temperatura.index_segundo] = temp_atual;

    // Incrementa os índices do segundo nível
    Tensao.index_segundo = (Tensao.index_segundo + 1) % SecondLevel_Length;
    Corrente.index_segundo = (Corrente.index_segundo + 1) % SecondLevel_Length;
    Temperatura.index_segundo = (Temperatura.index_segundo + 1) % SecondLevel_Length;

    // Verifica se o buffer do segundo nível está completo
    if (Tensao.index_segundo == 0) {
        int64_t buffer_end_time2 = esp_timer_get_time();
        int64_t total_buffer_time2 = buffer_end_time2 - buffer_start_time2;

        ESP_LOGI("Timing", "Tempo do segundo nível: %lld us", total_buffer_time2);

        float media_v2 = calcular_media(Tensao.SegundaCamada, SecondLevel_Length);
        float media_i2 = calcular_media(Corrente.SegundaCamada, SecondLevel_Length);
        Temperatura.media = calcular_media(Temperatura.SegundaCamada, SecondLevel_Length);
        ESP_LOGI(Temp, "Média do segundo nível de temperatura: %.2f", Temperatura.media);


        // ---- Terceiro nivel --------


        // Armazena as médias no terceiro nível
        Tensao.TerceiraCamada[Tensao.index__Terceiro] = media_v2;
        Corrente.TerceiraCamada[Corrente.index__Terceiro] = media_i2;

        // Incrementa os índices do terceiro nível
        Tensao.index__Terceiro = (Tensao.index__Terceiro + 1) % TerceiraCamada_Length;
        ESP_LOGI("Third_Level", "Índice do terceiro nível de tensão: %d", Tensao.index__Terceiro);

        Corrente.index__Terceiro = (Corrente.index__Terceiro + 1) % TerceiraCamada_Length;
        ESP_LOGI("Third_Level", "Índice do terceiro nível de corrente: %d", Corrente.index__Terceiro);

        // Adicione uma variável estática para armazenar o tempo de início do terceiro nível
        static int64_t buffer_start_time3 = 0;

        // Dentro da verificação do terceiro nível
        if (Tensao.index__Terceiro == 0) {
            // Se o índice for 0, significa que o terceiro nível foi preenchido
            int64_t buffer_end_time3 = esp_timer_get_time();
            int64_t total_buffer_time3 = buffer_end_time3 - buffer_start_time3;

            // Log do tempo total do terceiro nível
            ESP_LOGI("Timing", "Tempo do terceiro nível: %lld us", total_buffer_time3);

            // Reinicia o tempo de início para o próximo ciclo
            buffer_start_time3 = esp_timer_get_time();

            // Calcula a média do terceiro nível
            float soma_v3 = 0.0, soma_i3 = 0.0;
            for (int i = 0; i < TerceiraCamada_Length; i++) {
                soma_v3 += Tensao.TerceiraCamada[i];
                soma_i3 += Corrente.TerceiraCamada[i];
            }
            float media_v3 = soma_v3 / TerceiraCamada_Length;
            float media_i3 = soma_i3 / TerceiraCamada_Length;

            ESP_LOGI(V_Third, "Média do terceiro nível de tensão: %.3f V", media_v3);
            ESP_LOGI(I_Third, "Média do terceiro nível de corrente: %.3f A", media_i3);
        } else {
            // Inicializa o tempo de início na primeira execução
            if (buffer_start_time3 == 0) {
                buffer_start_time3 = esp_timer_get_time();
            }
        }
    }
}
