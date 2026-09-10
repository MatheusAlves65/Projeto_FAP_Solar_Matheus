#ifndef Monitoring_H
#define Monitoring_H



 //extern Energy_ADC Voltage;
 //extern Energy_ADC Current;
 //extern Temp_ADC Temperatura;
 //extern float calcular_media(const float *vetor, int tamanho);

extern volatile bool buffer_completo; // Declaração da variável global
extern volatile bool gravando_evento;
/**
* @brief Define a estrtura de dados steady_state_measure para agregar todas as medudas 
* a serem enviadas como medição de Regime Permanente para o backend.
*/


/**
 * @brief Realiza a aquisição instantânea do ADC e atualiza os buffers do primeiro nível
 */
void Instantaneous_Energy(void);
/**
 * @brief Processa os dados após a aquisição, atualizando os buffers dos níveis superiores e gerenciando eventos de limite
 */
void Continuous_Energy(void);
/**
 * @brief Inicializa os parâmetros dos ADCs de energia e eventos associados
 */
void Initialize_Parameters(void);


#endif



