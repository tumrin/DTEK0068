#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "lcd.h"
#include <stdio.h>
#include "adc.h"
#include "display.h"
#include "usart.h"

void display_task(void *param)
{
    ADC_result_t adc_results;
    
    vTaskDelay(200);
    
    for(;;)
    {
        xSemaphoreTake(mutex_handle, 100);
        adc_results = read_adc();
        xQueueSend(lcd_queue, &adc_results, 10);
        xSemaphoreGive(mutex_handle);
        vTaskDelay(100);
    }
    vTaskDelete(NULL);
}
