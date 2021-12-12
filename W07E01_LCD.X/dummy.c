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
    xSemaphoreTake(mutex_handle, 100);
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
    PORTF.DIRSET = PIN5_bm;
    TimerHandle_t led_timer = xTimerCreate
      ( /* Just a text name, not used by the RTOS
        kernel. */
        "led",
        /* The timer period in ticks, must be
        greater than 0. */
        100,
        /* The timers will auto-reload themselves
        when they expire. */
        pdTRUE,
        /* The ID is used to store a count of the
        number of times the timer has expired, which
        is initialised to 0. */
        ( void * ) 5,
        /* Each timer calls the same callback when
        it expires. */
        led_timer_callback);
    
    xTimerStart(led_timer, 0);
    vTaskDelay(200);
    for(;;)
    {
    }
    vTaskDelete(NULL);
    
}