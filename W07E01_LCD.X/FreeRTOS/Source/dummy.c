#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 

void dummy_task(void *param)
{
    vTaskDelay(200);
    for(;;)
    {
        
    }
    vTaskDelete(NULL);
    
}