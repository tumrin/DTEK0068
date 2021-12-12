#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include "timers.h"

ADC_result_t adc_result;
uint16_t last_pot;
TimerHandle_t backlight_time;

void backlight_timer_callback()
{
    if(adc_result.ldr > ((1023 * 2) / 3))
    {
        TCB3.CCMP = 0x80FF;
    }
    else
    {
        TCB3.CCMP = 0x20FF;
    }
}
void timeout_timer_callback()
{
    if(xTimerIsTimerActive(backlight_time) == pdTRUE)
    {
        xTimerStop(backlight_time, portMAX_DELAY);
    }
    TCB3.CCMP = 0;
}

/** Task for controlling backlight on lcd
 * 
 * @param param
 */
void backlight_task(void *param)
{
    backlight_time = xTimerCreate
    (
        "Backlight",
        100,
        pdTRUE,
        ( void * ) 3,
        backlight_timer_callback
    );
    TimerHandle_t timeout_time = xTimerCreate
    (
        "timeout",
        10000,
        pdFALSE,
        ( void * ) 4,
        timeout_timer_callback
    );
    xTimerStart(backlight_time, portMAX_DELAY);
    
    vTaskDelay(200);
    for(;;)
    {
        xSemaphoreTake(mutex_handle, 100);
        adc_result = read_adc();
        xSemaphoreGive(mutex_handle);
        
        if(last_pot == adc_result.pot)
        {
            if(xTimerIsTimerActive(timeout_time) == pdFALSE)
            {
                xTimerStart(timeout_time, portMAX_DELAY);
            }
        }
        else{
            if(xTimerIsTimerActive(backlight_time) == pdFALSE)
            {
                xTimerStart(backlight_time, portMAX_DELAY);
            }
            if(xTimerIsTimerActive(timeout_time) == pdTRUE)
            {
                xTimerStop(timeout_time, portMAX_DELAY);
            }
            last_pot = adc_result.pot;
        }
    }
    vTaskDelete(NULL);
}
void backlight_init()
{
    PORTB.DIRSET = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
}