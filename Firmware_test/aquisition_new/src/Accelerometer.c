#include <stdio.h>
#include <stdint.h>
#include "Accelerometer.h"
#include "esp_log.h"
#include "driver/i2c.h"


static const char *TAG = "ADXL345";





void adxl345_init(int sda_pin, int scl_pin) {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        .clk_flags = 0
    };
    
    // Configura o I2C
    if (i2c_param_config(I2C_MASTER_NUM, &conf) != ESP_OK) {
        ESP_LOGE(TAG, "Erro na config do I2C");
        return;
    }
    
    if (i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao instalar driver I2C");
        return;
    }

    // Configura o sensor para medir (POWER_CTL -> Measure bit)
    uint8_t power_on[] = {0x2D};
    esp_err_t err = i2c_master_write_to_device(I2C_MASTER_NUM, ADXL345_ADDR, power_on, sizeof(power_on), 1000 / portTICK_PERIOD_MS);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Sensor não respondeu!");
    } else {
        ESP_LOGI(TAG, "ADXL345 Inicializado.");
    }

    //COnfigura pinos de interrupção
    gpio_set_pull_mode(GPIO_INT_PIN, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_INT2_PIN, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_INT_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_INT2_PIN, GPIO_MODE_INPUT);
}

void adxl345_read_accel(adxl345_data_t *data) {
    uint8_t reg = 0x32; // DATAX0
    uint8_t raw[6];

    // Tenta ler
    esp_err_t err = i2c_master_write_read_device(I2C_MASTER_NUM, ADXL345_ADDR, &reg, 1, raw, 6, 1000 / portTICK_PERIOD_MS);

    if (err == ESP_OK) {
        // Se deu certo, preenche os dados
        data->x = (int16_t)((raw[1] << 8) | raw[0]);
        data->y = (int16_t)((raw[3] << 8) | raw[2]);
        data->z = (int16_t)((raw[5] << 8) | raw[4]);
    } else {
        // Se deu erro, zera tudo ou imprime erro (opcional)
        ESP_LOGE(TAG, "Falha na leitura");
        data->x = 0;
        data->y = 0;
        data->z = 0;
    }
}

