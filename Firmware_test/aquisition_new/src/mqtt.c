#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "cJSON.h"



#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"
#include "Monitoring.h"
#include "Events.h"
#include "MOSFET.h"
#define Pin_osc 20

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_conectado = false;

//void task_telemetria(void *pvParams);

static const char *TAG = "MQTT";


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "v1/devices/me/rpc/request/+", 1);
        esp_mqtt_client_subscribe(client, "v1/devices/me/attributes", 1);
        ESP_LOGI(TAG, "Inscrito no tópico de comandos RPC");
       msg_id = esp_mqtt_client_subscribe(client, "topic/qos0", 0);
        //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        //msg_id = esp_mqtt_client_subscribe(client, "topic/qos1", 1);
        //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        mqtt_conectado = true;

        //msg_id = esp_mqtt_client_unsubscribe(client, "topic/qos1");
        //ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        //xTaskCreate(task_telemetria, "task_telemetria", 8192, client, 5, NULL);
        //ESP_LOGI(TAG, "Tarefa de telemetria criada");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqtt_conectado = false;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        msg_id = esp_mqtt_client_publish(client, "topic/qos0", "data", 0, 0, 0);
       // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        //ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
case MQTT_EVENT_DATA:
        ESP_LOGW(TAG, "TÓPICO DE AÇÃO RECEBIDO: %.*s", event->topic_len, event->topic);
        ESP_LOGW(TAG, "PAYLOAD DE AÇÃO RECEBIDO: %.*s", event->data_len, event->data);

        // Verifica se a mensagem chegou no tópico de ATRIBUTOS
        if (strncmp(event->topic, "v1/devices/me/attributes", 24) == 0) {
            
            // Cria uma string segura (terminada em \0) baseada no payload recebido
            char *payload_str = strndup(event->data, event->data_len);
            
            if (payload_str != NULL) {
                // 1. Procura pela chave exata dentro da string do JSON
                char *posicao_chave = strstr(payload_str, "\"new_state_MOSFET\"");
                
                // Se o ponteiro não for NULL, significa que a palavra existe no pacote
                if (posicao_chave != NULL) {
                    char resposta_state[64];
                    
                    // 2. A partir de onde achou a chave, procura se o valor é "true"
                    if (strstr(posicao_chave, "true") != NULL) {
                        //ESP_LOGW("MOSFET", "Botão do Dashboard: LIGADO! Acionando Proteção (HIGH)");
                        Negative_Set_State(true);
                        Positive_Set_State(true);
                        Save_MOSFET_STATE_FLASH(true, true);
                        snprintf(resposta_state, sizeof(resposta_state), "{\"estado_MOSFET\": true}");
                        msg_id = esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", resposta_state, 0, 1, 0);
                    } 
                    // 3. Se não for true, procura se o valor é "false"
                    else if (strstr(posicao_chave, "false") != NULL) {
                        //ESP_LOGW("MOSFET", "Botão do Dashboard: DESLIGADO! Liberando Sistema (LOW)");
                        Negative_Set_State(false);
                        Positive_Set_State(false);
                        Save_MOSFET_STATE_FLASH(false, false);
                        snprintf(resposta_state, sizeof(resposta_state), "{\"estado_MOSFET\": false}");
                        msg_id = esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", resposta_state, 0, 1, 0);
                    }
                }
                
                // Libera a memória RAM 
                free(payload_str); 
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}



void mqtt_start(void) {
    ESP_LOGI(TAG, "Iniciando MQTT...");

    if (mqtt_client != NULL) {
        ESP_LOGW(TAG, "MQTT já iniciado");
        return;
    }

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.0.100:1883",
        .credentials.username = "Ak87nhneC00jiNWKV5X4",
        .buffer.out_size = 8192 // Tamanho do buffer de saída
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if (mqtt_client == NULL) {
        ESP_LOGE(TAG, "Falha ao criar cliente MQTT");
        return;
    }

    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
    
}
void mqtt_enviar_rotina(float tensao, float corrente, float temperatura)
{
    char payload[256];
    VerifyMOSFET_state(); // Verifica o estado do MOSFET antes de enviar a telemetria
    snprintf(payload, sizeof(payload),
             "{\"timestamp\": %ld, \"tensao\": %.2f, \"corrente\": %.2f, \"temperatura\": %.2f, \"estado_MOSFET\": %s}",
             (long)time(NULL), tensao, corrente, temperatura, VerifyMOSFET_state() ? "True" : "False");

    int msg_id = esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", payload, 0, 1, 0);

    if (msg_id >= 0) {
        ESP_LOGI("MQTT", "Telemetria de rotina enviada.");
        //ESP_LOGI("MQTT", "Payload Regime Permanente: %s", payload);
    } else {
        ESP_LOGE("MQTT", "Falha ao enviar rotina.");
    }
}

void mqtt_enviar_alerta(PacoteEvento_t pacote)
{
    // Aloca buffer na Heap para evitar Stack Overflow
    char *payload = (char *)malloc(8192);
    if (payload == NULL) {
        ESP_LOGE("MQTT", "Falha de memória ao alocar payload do evento!");
        return; 
    }

    int offset = 0;

    // 1. Início do JSON e chave "Tensao_array"
    offset += snprintf(payload, 8192,
             "{\"timestamp_alerta\": %lld, \"tipo\": \"%s\", \"Tensao_array\": [",
             pacote.timestamp, pacote.tipo);
             
    // Preenche os dados de Tensão
    for(int i = 0; i < NUM_amostras; i++) {
        offset += snprintf(payload + offset, 8192 - offset, "%.2f%s", 
                           pacote.tensao[i], (i < NUM_amostras - 1) ? ", " : "");
                           
        // Libera a CPU MOSFETamente a cada 50 iterações (Watchdog e Wi-Fi respiram)
        if (i % 50 == 0) {
            vTaskDelay(1);
        }
    }

    // 2. Fecha o array de Tensão e abre o "Corrente_array"
    offset += snprintf(payload + offset, 8192 - offset, "], \"Corrente_array\": [");

    // Preenche os dados de Corrente
    for (int i = 0; i < NUM_amostras; i++) {
        offset += snprintf(payload + offset, 8192 - offset, "%.2f%s", 
                           pacote.corrente[i], (i < NUM_amostras - 1) ? ", " : "");
                           
        // Libera a CPU novamente
        if (i % 50 == 0) {
            vTaskDelay(1);
        }
    }

    // 3. Fecha o array de Corrente e o JSON inteiro
    snprintf(payload + offset, 8192 - offset, "]}");

    // Publica no broker
    int msg_id = esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", payload, 0, 1, 0);

    if (msg_id >= 0) {
        //ESP_LOGW("MQTT", "Oscilografia ENVIADA com sucesso (Tamanho: %d bytes)", offset);
        ESP_LOGI("MQTT", "Payload de evento: %s", payload);
    } else {
        ESP_LOGE("MQTT", "Falha ao enviar Oscilografia (Estourou buffer?)");
    }

    // Libera a memória dinâmica após o uso
    free(payload);
}
void mqtt_alerta_off(int64_t timestamp_fim, int64_t duracao_ms, const char* tipo) {
    char payload[256];
    
    // Agora enviamos o "tipo" junto com a duração e o NORMAL!
    snprintf(payload, sizeof(payload),
             "{\"timestamp\": %lld, \"status\": \"NORMAL\", \"duracao_real\": %lld, \"tipo\": \"%s\"}",
             timestamp_fim, duracao_ms, tipo);
    
    int msg_id = esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", payload, 0, 1, 0);

    if (msg_id >= 0) {
                ESP_LOGI("MQTT", "payload ALERTA enviado com sucesso. Tipo: %s", tipo);
        ESP_LOGI("MQTT", "Payload Alerta Off: %s", payload);
        ESP_LOGI("MQTT", "Alerta OFF enviado. Duração: %lld ms | Tipo: %s", duracao_ms, tipo);
    }
}
    
void mqtt_alerta_on(int64_t timestamp, const char* tipo) {
    char payload[128];
    // Envia o status ALERTA de forma isolada, leve e rápida
    snprintf(payload, sizeof(payload), 
             "{\"timestamp\": %lld, \"status\": \"ALERTA\", \"tipo\": \"%s\",\"estado_MOSFET\": %s}", 
             timestamp, tipo, VerifyMOSFET_state() ? "True" : "False");
    
    int msg_id = esp_mqtt_client_publish(mqtt_client, "v1/devices/me/telemetry", payload, 0, 1, 0);
    if (msg_id >= 0) {
        ESP_LOGI("MQTT", "payload ALERTA enviado com sucesso. Tipo: %s", tipo);
        ESP_LOGI("MQTT", "Payload Alerta ON: %s", payload);
        ESP_LOGI("MQTT", "Gatilho de ALERTA iniciado!");
    }
}






