#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "lcd.h"
#include <stdio.h>
#include "adc.h"
#include "display.h"
#include "usart.h"

/** Task for reading adc and sending values to lcd via queue
 * 
 * @param param
 */
void display_task(void *param)
{
    ADC_result_t adc_results;
    
    vTaskDelay(200);
    
    for(;;)
    {
        adc_results = read_adc(); // Read adc
        
        // Overwrite queue with new value
        xQueueOverwrite(lcd_queue, &adc_results);
        
        // Allow other tasks to run, doesn't seem to work without this
        vTaskDelay(100);
    }
    vTaskDelete(NULL);
}
