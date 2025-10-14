#include "libraries.h"
#include "esp_log.h"
#include <stdio.h>


int event_flag_counter = 0;
int after_event_counter = 0;

void event_init(event_object * event){
    event->state = 0;
    event->previous_state = 0;}