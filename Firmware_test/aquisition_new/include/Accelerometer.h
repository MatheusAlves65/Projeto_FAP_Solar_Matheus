#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#define I2C_MASTER_NUM 0
#define I2C_MASTER_FREQ_HZ 100000
#define ADXL345_ADDR 0x53
#define GPIO_INT_PIN 10 // Pino de interrupção do ADXL345
#define GPIO_INT2_PIN 11 // Pino de interrupção 2 do ADXL345
#define SDA_PIN 6
#define SCL_PIN 7

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} adxl345_data_t;
/**
 * @brief Inicializa o ADXL345 e o barramento I2C
 * @param sda_pin Pino SDA do I2C
 * @param scl_pin Pino SCL do I2C
 */
void adxl345_init(int sda_pin, int scl_pin);

/**
 * @brief Lê os valores de aceleração do ADXL345
 * @param data Ponteiro para estrutura onde os dados serão armazenados
 */
void adxl345_read_accel(adxl345_data_t *data);



#endif



/*COMO USAR:

// Seus pinos (Confirme no hardware)
#define SDA_PIN 6
#define SCL_PIN 7

void app_main(void) {
    
    // Chama a inicialização (se der erro, vai aparecer no terminal, mas não aqui)
    adxl345_init(SDA_PIN, SCL_PIN);

    adxl345_data_t sensor;

    while (1) {
        // Atualiza a estrutura sensor
        adxl345_read_accel(&sensor);

        printf("Aceleração -> X: %d  Y: %d  Z: %d\n", sensor.x, sensor.y, sensor.z);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}















*/