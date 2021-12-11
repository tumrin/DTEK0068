#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"

void backlight_task(void *param)
{
    vTaskDelay(200);
    for(;;)
    {
     
    }
    vTaskDelete(NULL);
}
void backlight_init()
{
    PORTB.DIRSET = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
}