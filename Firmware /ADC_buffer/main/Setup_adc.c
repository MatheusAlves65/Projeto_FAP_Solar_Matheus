#include "libraries.h"
#include "global.h"

int adc_value_2;
int adc_value_3;
int adc_value_1;

void adc_setup(void) {
    // Inicializar o ADC
    adc1_channel_t channel2 = ADC1_CHANNEL_2;
    adc1_channel_t channel3 = ADC1_CHANNEL_3;
    adc1_channel_t channel4 = ADC1_CHANNEL_4;
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configurar os canais ADC 2, 3 e 4, atenuação
    adc1_config_channel_atten(channel2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel4, ADC_ATTEN_DB_0);
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

        // Ajusta o tempo para garantir 0,2 segundos

        // Calcula a média do primeiro nível
        float soma_tensao = 0.0, soma_corrente = 0.0;
        for (int i = 0; i < PrimeiraCamada_Length; i++) {
            soma_tensao += Tensao.PrimeiraCamada[i];
            soma_corrente += Corrente.PrimeiraCamada[i];
        }
        Tensao.media = soma_tensao / PrimeiraCamada_Length;
        Corrente.media = soma_corrente / PrimeiraCamada_Length;

        ESP_LOGI(V_First, "Média calculada a cada 0.2s: %.3f V", Tensao.media);
        ESP_LOGI(I_First, "Média calculada a cada 0.2s: %.3f A", Corrente.media);

        buffer_completo = true;
    }
}

void After_Acquisition() {
    static int64_t buffer_start_time2 = 0;

    // Inicializa o tempo de início quando o índice é 0
    if (Tensao.index_segundo == 0) {
        buffer_start_time2 = esp_timer_get_time();
    }

    // Armazena as médias do primeiro nível no segundo nível
    Tensao.SegundaCamada[Tensao.index_segundo] = Tensao.media;
    Corrente.SegundaCamada[Corrente.index_segundo] = Corrente.media;

    // Incrementa os índices do segundo nível
    Tensao.index_segundo = (Tensao.index_segundo + 1) % SecondLevel_Length;
    Corrente.index_segundo = (Corrente.index_segundo + 1) % SecondLevel_Length;

    // Verifica se o buffer do segundo nível está completo
    if (Tensao.index_segundo == 0) {
        int64_t buffer_end_time2 = esp_timer_get_time();
        int64_t total_buffer_time2 = buffer_end_time2 - buffer_start_time2;

        // Log do tempo do segundo nível
        ESP_LOGI("Timing", "Tempo do segundo nível: %lld us", total_buffer_time2);

        // Calcula a média do segundo nível
        float soma_tensao = 0.0, soma_corrente = 0.0;
        for (int i = 0; i < SecondLevel_Length; i++) {
            soma_tensao += Tensao.SegundaCamada[i];
            soma_corrente += Corrente.SegundaCamada[i];
        }
        float media_v2 = soma_tensao / SecondLevel_Length;
        float media_i2 = soma_corrente / SecondLevel_Length;

        ESP_LOGI(V_Second, "Média do segundo nível de tensão: %.3f V", media_v2);
        ESP_LOGI(I_Second, "Média do segundo nível de corrente: %.3f A", media_i2);


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

void adc_init_temp(void) {
    adc_value_1 = adc1_get_raw(ADC1_CHANNEL_4);
    float temp_medida = (adc_value_1 / (float)ADC_Max_value) * V_REF; // Ajustado para cálculo correto
    ESP_LOGI(Temp, "Temperatura Medida: %.2f", temp_medida);
}