#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h> // Para obter o timestamp atual exato

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h" 

// Includes do seu projeto
#include "Monitoring.h"
#include "Temp.h"
#include "Energy.h"
#include "mqtt.h"
#include "MOSFET.h"
#include "Timer.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "Wifi.h"
#include "Events.h"

// Includes de Hardware
#include "driver/gpio.h"
#include "led_strip.h"

#define BLINK_GPIO 8
#define Pin_osc 21

// ============================================================================
// Variáveis Globais e Handles
// ============================================================================
TaskHandle_t Handle_Main_Measurement;
SemaphoreHandle_t Transmit; // Semáforo para sinalizar a tarefa de transmissão de rotina
static led_strip_handle_t led_strip;
int64_t timestamp_inicio_evento;

// Variáveis externas vindas de outros ficheiros (Energy.c, Monitoring.c, etc.)
extern volatile bool buffer_completo; 
extern volatile bool event_flag_3s;   
extern volatile bool log_pendente;    
extern volatile bool gravando_evento; 
extern Energy_ADC Voltage;
extern Energy_ADC Current;
extern Temp_ADC Temperatura;
extern int contador_eventos; 
extern volatile int64_t pior_tempo_execucao_adc; // Pior tempo de execução da aquisição instantânea (em microssegundos)

// ============================================================================
// Configuração do LED RGB da Placa
// ============================================================================
void configure_led(void) {
    ESP_LOGI("LED", "Configurando o LED RGB embutido");
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // Apenas um LED na placa
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, 
        .led_model = LED_MODEL_WS2812,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_clear(led_strip);
}

// ============================================================================
// Tarefa 1: Medição Principal (ADC e Processamento de Energia)
// ============================================================================
void Main_Measurement(void *param) {

    // Cria o semáforo binário para o Timer
    SemaphoreHandle_t timer_semaphore = xSemaphoreCreateBinary();
    if (timer_semaphore == NULL) {
        ESP_LOGE("Tasks", "Falha ao criar o semaphore do timer.");
        vTaskDelete(NULL);
    }

    // Inicialização do Hardware e Parâmetros
    adc_setup();
    MOSFET_Init();
    Initialize_Parameters();
    
    configure_led();
    led_strip_set_pixel(led_strip, 0, 0, 0, 255); // Azul (A aguardar ligação)
    led_strip_refresh(led_strip);
    
    // Inicia o Timer de aquisição (Chama a Instant_Acquisition na ISR)
    //config_gpios(); // Configura o pino do osciloscópio
    iniciar_timer(timer_semaphore); 

    int64_t tempo_anterior = esp_timer_get_time();
    //ESP_LOGI("Validação","Tempo inicial: %lld us", tempo_anterior);
    int64_t max_desvio_us = 0;
    uint32_t contador_amostras = 0;

    while (1) {
        // Aguarda o semáforo ser libertado pelo timer de hardware
        if (xSemaphoreTake(timer_semaphore, portMAX_DELAY) == pdTRUE) {

            int64_t tempo_atual = esp_timer_get_time();
            int64_t delta_t = tempo_atual - tempo_anterior;
            tempo_anterior = tempo_atual;

            int64_t desvio = delta_t - 1000;
            if (desvio < 0) desvio = -desvio; 

            if (desvio > max_desvio_us) {
                max_desvio_us = desvio;
            }

            contador_amostras++;
            
            // A cada 1000 amostras (aprox. 1 segundo), imprime o relatório e reseta[cite: 9]
            if (contador_amostras >= 1000) {
                ESP_LOGW("DIAGNOSTICO", "--- Relatorio de Desempenho (1s) ---");
                ESP_LOGI("DIAGNOSTICO", "1. Atraso de envio: %lld us", max_desvio_us);
                ESP_LOGI("DIAGNOSTICO", "2. Tempo de leitura do ADC : %lld us", pior_tempo_execucao_adc);
                
                // Zera as variáveis para analisar o próximo segundo
                max_desvio_us = 0;
                pior_tempo_execucao_adc = 0; 
                contador_amostras = 0;}

            
            // Verifica se o buffer de amostras encheu
            if (buffer_completo == true) {
                Continuous_Energy();
                
                // Rotina de 3 segundos para telemetria contínua
                if(event_flag_3s){
                    xSemaphoreGive(Transmit); // Sinaliza a tarefa de rede
                    event_flag_3s = false;
                }
                buffer_completo = false; // Reset da flag
            }
        }
    }
}

// ============================================================================
// Tarefa 2: Gestão de Rede e Máquina de Estados MQTT (Fila e Oscilografia)
// ============================================================================
void conectarWifi(void *pvParameters) {
    PacoteEvento_t pacote_recebido;
    
    bool evento_ativo_no_painel = false; 
    int64_t timestamp_inicio_real = 0;
    int64_t timestamp_fim_real = 0;
    bool fim_detectado = false;

    
    // NOVO: Variável para memorizar quem causou o problema
    char tipo_evento_ativo[32] = ""; 

    while (1) {
        // 1. RECEBE DA FILA
        if (xQueueReceive(fila_eventos, &pacote_recebido, pdMS_TO_TICKS(10)) == pdTRUE) {
            
            if (!evento_ativo_no_painel) {
                timestamp_inicio_real = timestamp_inicio_evento; // Usa o timestamp do início do evento registrado no Energy.c
                timestamp_inicio_real = pacote_recebido.timestamp;
                
                // NOVO: Guarda o nome do evento ativo ("Tensao" ou "Corrente")
                snprintf(tipo_evento_ativo, sizeof(tipo_evento_ativo), "%s", pacote_recebido.tipo);
                
                mqtt_alerta_on(timestamp_inicio_real, pacote_recebido.tipo);
                evento_ativo_no_painel = true; 
                fim_detectado = false; 
            }
            mqtt_enviar_alerta(pacote_recebido);
        }

        // 2. Detecta o fim do evento (quando ambos Voltage e Current voltam ao estado normal)
        if (evento_ativo_no_painel && !fim_detectado) {
            if (Voltage.state == true && Current.state == true) { 
                struct timeval tv;
                gettimeofday(&tv, NULL);
                timestamp_fim_real = (int64_t)tv.tv_sec * 1000LL + (int64_t)tv.tv_usec / 1000LL;
                fim_detectado = true;
            }
        }

        // 3. ENVIA O OFF (Agora enviando a variável tipo_evento_ativo)
        if (evento_ativo_no_painel && fim_detectado) {
            if (uxQueueMessagesWaiting(fila_eventos) == 0 && !gravando_evento) {
                int64_t duracao_ms = timestamp_fim_real - timestamp_inicio_real;
                
                // NOVO: Passa o tipo para a função MQTT fechar o evento certo
                mqtt_alerta_off(timestamp_fim_real, duracao_ms, tipo_evento_ativo);
                
                evento_ativo_no_painel = false;
            }
        }

        // 4. TRATAMENTO DA ROTINA
        if (xSemaphoreTake(Transmit, 0) == pdTRUE) {
            // 1. Captura o tempo exato antes de acionar a rede
            int64_t tempo_inicio_mqtt = esp_timer_get_time();
            // 2. Executa o envio (a função que supostamente causa o gargalo)
            mqtt_enviar_rotina(Voltage.media, Current.media, Temperatura.real_value);
            
            // 3. Captura o tempo logo após o envio terminar
            int64_t tempo_fim_mqtt = esp_timer_get_time(); 
            
            // 4. Calcula a duração total
            int64_t duracao_mqtt = tempo_fim_mqtt - tempo_inicio_mqtt;

            // 5. Imprime o resultado no monitor serial
            //ESP_LOGW("DIAGNOSTICO_REDE", "Tempo gasto no envio MQTT: %lld us", duracao_mqtt);
        }
    }
}

// ============================================================================
// Ponto de Entrada Principal (Main)
// ============================================================================
void app_main(void) {
    // 1. Inicializa o NVS (Non-Volatile Storage) - Obrigatório para o Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    MOSFET_Init(); // Inicializa o sistema MOSFET
    nvs_flash_init(); // Inicializa o NVS para salvar estados dos pinos
     VerifyMOSFET_state(); // Verifica e aplica o estado salvo dos pinos no NVS
    gpio_reset_pin(Pin_osc); // Garante que o pino esteja baixo no início da ISR
    gpio_set_direction(Pin_osc, GPIO_MODE_OUTPUT); // Configura o pino
    gpio_set_level(Pin_osc, 0); // Inicializa o pino em nível baixo

    // 2. Criação de Semáforos e Filas Globais
    Transmit = xSemaphoreCreateBinary(); 
    if (Transmit == NULL) {
        ESP_LOGE("Main", "Falha ao criar o semaphore de transmissão.");
        return;
    }
    Inicializar_Fila_Eventos(); 

    ESP_LOGI("Sistema", "Tamanho do heap disponível: %lu bytes", (unsigned long)esp_get_free_heap_size());
    
    // 3. Inicialização dos Serviços de Rede
    Wifi_Start();
    ///Sync_Time(); // Sincroniza o relógio via NTP a cada envio de rotina
    mqtt_start(); // Arranca o cliente MQTT UMA ÚNICA VEZ aqui!
    
    // 4. Criação das Tarefas do FreeRTOS
    // Core 0/1 geridos automaticamente a não ser que utilize xTaskCreatePinnedToCore
    xTaskCreate(Main_Measurement, "Medição_Principal", 4096, NULL, 2, &Handle_Main_Measurement);
    xTaskCreate(conectarWifi, "Task_Rede_MQTT", 16384, NULL, 1, NULL);
}