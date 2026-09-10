#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "Energy.h"
#include "esp_log.h"
#include "esp_log_color.h"
#include <driver/adc.h>
#include <esp_adc/adc_oneshot.h>
#include "esp_adc/adc_cali.h"
#include "Events.h"
#include "MOSFET.h"
#include <sys/time.h>
#include "esp_timer.h"


int adc_value_2;
extern volatile bool buffer_completo;
extern volatile bool event_flag_3s;
extern volatile bool log_pendente;
extern int64_t timestamp_inicio_evento;
int contador_MOSFET = 0; // Contador para desligar o MOSFET após 1 segundo

// Handle global para o ADC one-shot
adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t adc_cali_handle = NULL; // Handle global para calibração

void Energy_ADC_Init(Energy_ADC *p) {
    // Zera índices
    p->index_primeiro = 0;  // Indice da primeira camada
    p->index_segundo = 0;   // Indice da segunda camada
    p->index__Terceiro = 0; // Indice da terceira camada
    p->index__quarto = 0;   // Indice da quarta camada
    p->index__Quinto = 0;   // Indice da quinta camada

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
    p->state = true;
}

void adc_setup(void) {
    // Configuração do ADC em modo one-shot
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1, // qual barramento do adc utlizado, nesse caso o 1
        .ulp_mode = ADC_ULP_MODE_DISABLE, // Modo ultra low power desabilitado
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    // Configurar canais
    adc_oneshot_chan_cfg_t chan_cfg = { // configuração do ADC
        .bitwidth = ADC_BITWIDTH_12, // resolução do adc
        .atten = ADC_ATTEN_DB_12, // atenuação por ser 3.3V
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_2, &chan_cfg); // Corrente
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3, &chan_cfg); // Tensão
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_4, &chan_cfg); // Temperatura
    // Configura pull-downs para os pinos ADC usados
    gpio_set_pull_mode(GPIO_NUM_3, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLDOWN_ONLY);

    // Inicializa calibração
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle); // cria a curva de calibração
}

void Instant_Acquisition(Energy_ADC *p) {
    // Leitura one-shot do ADC
    adc_oneshot_read(adc1_handle, p->adc_channel, &adc_value_2); 

    // Calibração e conversão
    int voltage_mv = 0;
    adc_cali_raw_to_voltage(adc_cali_handle, adc_value_2, &voltage_mv);
    p->instant_value = voltage_mv / 1000.0f;
    p->instant_value = p->instant_value * p->ganho + p->offset; 

    // Armazena no buffer do primeiro nivel
    p->PrimeiraCamada[p->index_primeiro] = p->instant_value;
    p->index_primeiro = (p->index_primeiro + 1) % PrimeiraCamada_Length;

    if (p->state == true) { 
        // --- ESTADO NORMAL: Monitorando limite ---
        if (p->instant_value > p->max_threshold) {
            p->contador_amostras++; // 1 amostra ~= 1 milissegundo
            
            // Passo 1: Confirma se o valor ficou acima do limite pelo tempo exigido
            if (p->contador_amostras >= p->tempo_confirmacao_ms) {
                
                // EVENTO DETECTADO! Registra o início do problema.
                struct timeval tv1;
                gettimeofday(&tv1, NULL);
                timestamp_inicio_evento = (int64_t)tv1.tv_sec * 1000LL + (int64_t)tv1.tv_usec / 1000LL;
                
                p->state = false; // ESTADO MUDA PARA "EM EVENTO"
                log_pendente = true; 
                
                // Zera o cronômetro exclusivo do MOSFET para começar a contar o atraso
                p->MOSFET_active_ms = 0; 
            } 
        } 
        else {
            p->contador_amostras = 0; // Filtro de ruído: reseta o contador se a tensão cair
        }
    } 
    
    // --- ESTADO DE EVENTO ATIVO (p->state == false) ---
    else { 
        // Passo 2: Contagem do atraso para acionar o MOSFET
        // Defina o tempo que você quer esperar (ex: 200 ms)
        
        if (p->MOSFET_active_ms < p->tempo_acionamento_mosfet_ms) {
            p->MOSFET_active_ms++;
            
            if (p->MOSFET_active_ms == p->tempo_acionamento_mosfet_ms) {
                // Passo 3: Tempo de atraso atingido. Aciona a proteção física!
                Positive_Set_State(true);
                Negative_Set_State(true);
                Save_MOSFET_STATE_FLASH(true, true); // Salva o estado do MOSFET na Flash
            }
        }

        // Passo 4: Lógica de Histerese para desativar e voltar ao normal
        if (p->instant_value < p->min_threshold) {
            p->contador_amostras++; // Reaproveita o contador para validar a volta ao normal
            
            // Exige que o sinal fique abaixo do limite por X milissegundos antes de rearmar
            if (p->contador_amostras > 50) { 
                p->state = true;          // VOLTA PARA "NORMAL"
                p->contador_amostras = 0; 
                log_pendente = false;      
            }
        } else {
            p->contador_amostras = 0; // Zera se houver pico de ruído durante a queda
        }
    }

    // Se o buffer do primeiro nivel está cheio, calcula a média e sinaliza
    if (p->index_primeiro == 0) {
        p->media = calcular_media(p->PrimeiraCamada, PrimeiraCamada_Length);
        buffer_completo = true;
    }
}

void After_Acquisition(Energy_ADC *p) {
    // Armazena as médias do primeiro nivel no segundo nivel
    p->SegundaCamada[p->index_segundo] = p->media;

    p->index_segundo++;

    // Verifica se o buffer do segundo nivel está completo
    if (p->index_segundo == SecondLevel_Length) {
        // ESP_LOGI("BUFFER2", "[%s]Buffer do segundo nivel completo.\n",p->tipo);

        // ---- Terceira Camada --------

        p->TerceiraCamada[p->index__Terceiro] = calcular_media(p->SegundaCamada, SecondLevel_Length); // buffer da terceira camada recebe a média do segundo nivel

        event_flag_3s = true; // Sinaliza que o terceiro nível foi atualizado, para verificação de eventos na tarefa principal

        p->valor_3s = p->TerceiraCamada[p->index__Terceiro]; // Armazena o valor do terceiro nível para uso na tarefa principal

        p-> index__Terceiro++;

        p-> index_segundo = 0;
    }
        // Inicio da medição do Terceiro nivel, que é o buffer de 3 segundos. Quando ele encher, será calculada a média e armazenada no quarto nível.

    if (p->index__Terceiro == TerceiraCamada_Length) {

        // ---- Quarta Camada --------
       ESP_LOGI("BUFFER3", "[%s] Buffer do terciro nivel completo. Média: %.3f\n", p->tipo, calcular_media(p->TerceiraCamada, TerceiraCamada_Length));
        // Quarto nivel
            p->QuartaCamada[p->index__quarto] = calcular_media(p->TerceiraCamada, TerceiraCamada_Length);
        // Incrementa os indices do Quarto nivel

        p->index__quarto++;

        p-> index__Terceiro = 0;

        }

    if (p->index__quarto == QuartaCamada_Length)    {

        // --- Quinta Camada ----
        // Calculo da média do terceiro nivel e armazenando no index do quarto
        p->QuintaCamada[p->index__Quinto] = calcular_media(p->QuartaCamada, QuartaCamada_Length);
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