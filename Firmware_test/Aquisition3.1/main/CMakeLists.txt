idf_component_register(SRCS "Events.c" "main_funcs.c" "Temp.c" "Energy.c" "Tasks.c" "Timer.c" "main.c" "global.c"
                    INCLUDE_DIRS "."
REQUIRES driver led_strip esp_adc nvs_flash esp_wifi esp_event esp_timer)