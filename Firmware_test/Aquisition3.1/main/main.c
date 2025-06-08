#include "libraries.h"
#include "global.h"



TaskHandle_t Handle_Main_Measurement;


void app_main(void)
{
xTaskCreate(Main_Measurement, "Medição_Principal", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_Main_Measurement);

}