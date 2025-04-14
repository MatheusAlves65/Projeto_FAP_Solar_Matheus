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

    // Configurar os canais ADC 2, 3 e 4, atenuaçao
    adc1_config_channel_atten(channel2, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel3, ADC_ATTEN_DB_0);
    adc1_config_channel_atten(channel4, ADC_ATTEN_DB_0);
}

void adc_init_Reading() {
    adc_value_2 = adc1_get_raw(ADC1_CHANNEL_2);
    adc_value_3 = adc1_get_raw(ADC1_CHANNEL_3);
    medidas.tensao_medida = ((float)adc_value_2 /ADC_Max_value) * V_REF;
    medidas.corrente_medida = ((float)adc_value_3 /ADC_Max_value) * V_REF;

    // Debug: Mostra valor sendo armazenado e posição atual
    printf("Armazenando tensão %.3f V na posição %d do buffer\n", 
           medidas.tensao_medida, medidas.index_primeiro);

    // Armazena a tensão medida no buffer firstLevel
    medidas.PrimeiraCamada[medidas.index_primeiro] = medidas.tensao_medida;

    // Incrementa o índice e faz o wrap-around quando chegar ao final
    medidas.index_primeiro = (medidas.index_primeiro + 1) % PrimeiraCamada_Length;

    if (medidas.index_primeiro >= 191) {
        printf("\n--- Início do processamento do buffer ---\n");
        medidas.index_primeiro = 0;
        
        // Calcula a média do primeiro nível
        float soma = 0.0;
        for(int i = 0; i < PrimeiraCamada_Length; i++) {
            soma += medidas.PrimeiraCamada[i];
            // Debug: Mostra a cada 32 amostras somadas
            if ((i + 1) % 32 == 0) {
                printf("Soma parcial após %d amostras: %.3f\n", i + 1, soma);
            }
        }
        float media = soma / PrimeiraCamada_Length;
        
        // Debug: Mostra detalhes do cálculo
        printf("Soma total: %.3f\n", soma);
        printf("Média calculada: %.3f V\n", media);
        
        // Armazena a média no segundo nível
        medidas.SegundaCamada[medidas.index__segundo] = media;
        
        // Debug: Mostra onde foi armazenado no segundo nível
        printf("Média armazenada na posição %d do segundo nível\n", 
               medidas.index__segundo);
        
        // Incrementa o índice do segundo nível
        medidas.index__segundo = (medidas.index__segundo + 1) % SecondLevel_Length;
        printf("Armazenando tensão %.3f V na posição %d do Segundo nivel\n", 
            media, medidas.index__segundo);
        
        medidas.primeiro_nivel_ciclos++;
        printf("Ciclo %lu completado\n", (unsigned long)medidas.primeiro_nivel_ciclos);
        printf("--- Fim do processamento do buffer ---\n\n");
    }
}







void adc_init_temp (void) {  // Configurando a resolução do ADC para leitura da temperatura
     adc_value_1 = adc1_get_raw(ADC1_CHANNEL_4);
    float temp_medida = (adc_value_1 / (float)ADC_Max_value) * Max_Value_Voltage; // arrumar!!!!
    printf("Temperatura Medida: %.2f\n", temp_medida);
}