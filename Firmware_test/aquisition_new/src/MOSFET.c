#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> 
#include "MOSFET.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nvs_flash.h"


void MOSFET_Init(void) {
    // 1. Libera os pinos de funções especiais de hardware (Desvincula o USB do pino 12)
    gpio_reset_pin(Negative_PIN);
    gpio_reset_pin(Positive_PIN);

    // 2. Configura os pinos como saída
    gpio_set_direction(Negative_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(Positive_PIN, GPIO_MODE_OUTPUT);
    
    // 3. Aplica os pull-downs
    gpio_set_pull_mode(Negative_PIN, GPIO_PULLDOWN_ONLY);
    gpio_set_pull_mode(Positive_PIN, GPIO_PULLDOWN_ONLY);
    
}

void Save_MOSFET_STATE_FLASH(bool negative_state, bool positive_state) {

    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Abre o namespace "MOSFET_state" para escrita
    nvs_handle_t my_handle;
    ret = nvs_open("MOSFET_state", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Erro ao abrir NVS para escrita!");
        return;
    }

    // Salva os estados dos pinos
    ret = nvs_set_u8(my_handle, "negative_state", negative_state ? 1 : 0);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Erro ao salvar estado do pino negativo!");
        nvs_close(my_handle);
        return;
    }

    ret = nvs_set_u8(my_handle, "positive_state", positive_state ? 1 : 0);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Erro ao salvar estado do pino positivo!");
        nvs_close(my_handle);
        return;
    }

    // Commit das alterações
    ret = nvs_commit(my_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Erro ao commitar alterações no NVS!");
    }

    // Fecha o handle do NVS
    nvs_close(my_handle);
}

bool VerifyMOSFET_state(void) {
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Abre o namespace "MOSFET_state" para leitura
    nvs_handle_t my_handle;
    ret = nvs_open("MOSFET_state", NVS_READONLY, &my_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("NVS", "Erro ao abrir NVS para leitura!");
    }

    // Lê os estados dos pinos
    uint8_t negative_state = 0;
    uint8_t positive_state = 0;

    ret = nvs_get_u8(my_handle, "negative_state", &negative_state);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("NVS", "Erro ao ler estado do pino negativo!");
        nvs_close(my_handle);
    }

    ret = nvs_get_u8(my_handle, "positive_state", &positive_state);
    if (ret != ESP_OK && ret != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE("NVS", "Erro ao ler estado do pino positivo!");
        nvs_close(my_handle);
    }
        bool estado_MOSFET = (negative_state == 1 && positive_state == 1) ? true : false;
    // Fecha o handle do NVS
    nvs_close(my_handle);

    // Aplica os estados lidos aos pinos
    Negative_Set_State(negative_state ? true : false);
    Positive_Set_State(positive_state ? true : false);

    //ESP_LOGI("MOSFET", "Estado do pino negativo: %s", negative_state ? "HIGH" : "LOW");
    //ESP_LOGI("MOSFET", "Estado do pino positivo: %s", positive_state ? "HIGH" : "LOW");
    return estado_MOSFET;
}

void Negative_Set_State(bool state) {
    if (state == true) {
        gpio_set_level(Negative_PIN, 1);
    } else {
        gpio_set_level(Negative_PIN, 0);
    }
}

void Positive_Set_State(bool state) {
    if (state == true) {
        gpio_set_level(Positive_PIN, 1);
    } else {
        gpio_set_level(Positive_PIN, 0);
    }
}