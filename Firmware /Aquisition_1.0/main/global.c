#include "global.h"
#include <stdbool.h>

volatile bool buffer_completo = false; // Definição da variável global


// Função para calcular media

float calcular_media(const float *vetor, int tamanho) {
    float soma = 0.0f;
    for (int i = 0; i < tamanho; i++) {
        soma += vetor[i];
    }
    return soma / tamanho;
}