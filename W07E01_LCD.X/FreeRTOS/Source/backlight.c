#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"

void backlight_task(void *param)
{
    ADC_result_t adc_result;
    
    
    vTaskDelay(200);
    for(;;)
    {
        adc_result = read_adc();
        TCB3.CCMP = adc_result.ldr;
    }
    vTaskDelete(NULL);
}
void backlight_init()
{
    PORTB.DIRSET = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
}