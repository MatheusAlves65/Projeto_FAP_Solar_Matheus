#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "Wifi.h"
#include "esp_sntp.h"

// --- CONFIGURAÇÕES DA REDE ---
#define WIFI_SSID         "Honda Deco"
#define WIFI_PASSWORD      "anayuri1"
#define WIFI_MAXIMUM_RETRY 5

// --- FLAGS DO EVENT GROUP ---
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "WIFI_owner";

static EventGroupHandle_t s_wifi_event_group;
static int s_retry_num = 0;

// --- HANDLER DE EVENTOS (WIFI E IP) ---
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data) 
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Tentando reconectar ao Wi-Fi... (%d/%d)", s_retry_num, WIFI_MAXIMUM_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Endereço IP Recebido: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Função de callback que o SNTP vai chamar automaticamente sempre que sincronizar
void cb_notificacao_sincronismo(struct timeval *tv) {
    time_t agora = tv->tv_sec;
    struct tm timeinfo = { 0 };
    localtime_r(&agora, &timeinfo);
    
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%d/%m/%Y - %H:%M:%S", &timeinfo);
    
    ESP_LOGI("NTP_AUTO", "Sincronização periódica invisível concluída! %s", strftime_buf);
}

// --- FUNÇÃO PRINCIPAL ---
void Wifi_Start() 
{
    // 1. Inicializa o NVS (Fundamental para a biblioteca de Wi-Fi do ESP32)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Criação do Grupo de Eventos e Loop de Sistema
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // 3. Inicialização e configuração do Wi-Fi
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));

    // 4. Registro dos tratadores de eventos
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        }
    };
    
    // 5. Início do Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    //esp_wifi_set_max_tx_power(8);// define minimo de potência para 8 dBm (aprox. 6,3 mW) para economizar energia

    ESP_LOGI(TAG, "wifi_init_sta finalizado. Aguardando conexão...");

    // 6. Aguarda até o Wi-Fi conectar com sucesso ou esgotar as tentativas
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    // 7. Lógica após tentativa de conexão (Sucesso ou Falha)
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Conectado com sucesso ao SSID:%s", WIFI_SSID);

        // =========================================================
        //                 INÍCIO DO SINCRONISMO NTP
        // =========================================================
        ESP_LOGI("timestamp", "Aguardando o roteador liberar a internet (DHCP/DNS)...");
        vTaskDelay(3000 / portTICK_PERIOD_MS); // Dá 3 segundos cruciais para a rede estabilizar

        ESP_LOGI("timestamp", "Inicializando SNTP com IPs fixos...");

        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        
        // Servidor 0: Nome de domínio padrão (Pode falhar se o DNS estiver lento)
        esp_sntp_setservername(0, "pool.ntp.org");
        
        // Servidor 1: IP BRUTO do NTP.br (Servidor oficial do Brasil - Não precisa de DNS)
        esp_sntp_setservername(1, "200.160.7.186"); 
        
        // Servidor 2: IP BRUTO do Google NTP (Backup de altíssima disponibilidade)
        esp_sntp_setservername(2, "216.239.35.0");

        sntp_set_sync_interval(3600000); // 3.600.000 ms = 1 hora de intervalo entre sincronizações automáticas

        //esp_sntp_set_time_sync_notification_cb(cb_notificacao_sincronismo);
        esp_sntp_init();

        setenv("TZ", "BRT3", 1); 
        tzset();

        time_t now = 0;
        struct tm timeinfo = { 0 };
        int retry = 0;
        const int retry_count = 15;
        
        while (timeinfo.tm_year < (2020 - 1900) && ++retry <= retry_count) {
            ESP_LOGI("timestamp", "Buscando hora na internet... (%d/%d)", retry, retry_count);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            time(&now);
            localtime_r(&now, &timeinfo);
        }

        if (timeinfo.tm_year < (2020 - 1900)) {
            ESP_LOGE("timestamp", "FALHA CRÍTICA: Placa conectou no Wi-Fi, mas sem acesso à internet ou porta UDP 123 bloqueada.");
        } else {
            char strftime_buf[64];
            strftime(strftime_buf, sizeof(strftime_buf), "%d/%m/%Y - %H:%M:%S", &timeinfo);
            ESP_LOGW("timestamp", "=========================================");
            ESP_LOGW("timestamp", "Relógio sincronizado! Tempo atual: %s", strftime_buf);
            ESP_LOGW("timestamp", "=========================================");
        }
        // =========================================================
        // =========================================================

    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Falha irreparável ao conectar no SSID:%s", WIFI_SSID);
    } else {
        ESP_LOGE(TAG, "EVENTO INESPERADO no Wi-Fi");
    }
}

