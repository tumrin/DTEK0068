#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "semphr.h"
#include "adc.h"
#include "timers.h"

// Timer callback
void led_timer_callback()
{
    xSemaphoreTake(mutex_handle, portMAX_DELAY);
    ADC_result_t adc_result = read_adc();
    xSemaphoreGive(mutex_handle);
    if(adc_result.ntc > adc_result.pot)
    {
        PORTF.OUTSET = PIN5_bm;
    }
    else
    {
        PORTF.OUTCLR = PIN5_bm;
    }
}

// Task
void dummy_task(void *param)
{
    PORTF.DIRSET = PIN5_bm; // Set PF5 to output
    
    TimerHandle_t led_timer = xTimerCreate
    (
        "Led",
        100,
        pdTRUE,
        ( void * ) 5,
        led_timer_callback
    );
    
    xTimerStart(led_timer, portMAX_DELAY);
    
    vTaskDelay(200);
    
    for(;;)
    {
        vTaskDelay(100);
    }
    vTaskDelete(NULL);
    
}