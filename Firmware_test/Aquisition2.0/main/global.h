#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

//bool buffer_completo;



//Tags para o LOG da ESP32
//static const char *Instant_v = "Instant_Voltage"; // Tag para identificar os logs da função Instant_Acquisition p/ tensão
//static const char *Instant_i = "Instant_Current"; // Tag para identificar os logs da função Instant_Acquisition p/ Corrente
//static const char *After_INSTANT = "AFTER_INSTANT"; // Tag para identificar os logs da função After_Acquisition
//static const char *Temp = "Temperature"; // Tag para identificar os logs da função After_Acquisition
//static const char *Tasks = "Inside_Task"; // Tag para identificar os logs das tasks
//static const char *TIMER = "Tempo_Func"; // Tag para identificar os logs das tasks

// Tags para Tensão (V)
//static const char *V_First = "V_FirstLevel";    // Primeiro nível - buffer instantâneo
//static const char *V_Second = "V_SecondLevel";  // Segundo nível - médias do primeiro
//static const char *V_Third = "V_ThirdLevel";    // Terceiro nível - processamento adicional
//static const char *V_Fourth = "V_FourthLevel";  // Quarto nível - agregação de dados
//static const char *V_Fifth = "V_FifthLevel";    // Quinto nível - resultado final

// Tags para Corrente (I)
//static const char *I_First = "I_FirstLevel";    // Primeiro nível - buffer instantâneo
//static const char *I_Second = "I_SecondLevel";  // Segundo nível - médias do primeiro
//static const char *I_Third = "I_ThirdLevel";    // Terceiro nível - processamento adicional
//static const char *I_Fourth = "I_FourthLevel";  // Quarto nível - agregação de dados
//static const char *I_Fifth = "I_FifthLevel";    // Quinto nível - resultado final

float calcular_media(const float *vetor, int tamanho);



#endif // GLOBAL_H