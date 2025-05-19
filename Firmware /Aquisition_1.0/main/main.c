#include "libraries.h"
#include "global.h"



TaskHandle_t Handle_voltage_read;





void app_main(void)
{
xTaskCreate(voltage_read, "voltage_read", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_voltage_read);

}