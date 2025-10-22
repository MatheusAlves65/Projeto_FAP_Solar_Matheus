#include "libraries.h"
#include "global.h"
#include "led_strip.h"



TaskHandle_t Handle_Main_Measurement;



void app_main(void)
{
    gpio_set_direction(GPIO_NUM_8, GPIO_MODE_OUTPUT);
    xTaskCreate(Main_Measurement, "Medição_Principal", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_Main_Measurement);

}
