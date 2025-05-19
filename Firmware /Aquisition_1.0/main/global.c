#include "global.h"

// Definindo a variável global para armazenar as medições do ADC
Energy_ADC Tensao;
Energy_ADC Corrente;
Temp_ADC Temperatura;



// Definição da variável global
bool buffer_completo = false;


// Função para calcular media

float calcular_media(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}