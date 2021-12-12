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
    uint16_t ratio = adc_result.ldr*10/1023;
    TCB3.CCMP = 0xFFFF - (0xFFFF/10*ratio);
    xTimerStart(backlight_time, 0);
}
void timeout_timer_callback()
{
    xTimerStop(backlight_time, 0);
    TCB3.CCMP = 0;
}

void backlight_task(void *param)
{
    backlight_time = xTimerCreate
      ( /* Just a text name, not used by the RTOS
        kernel. */
        "Backlight",
        /* The timer period in ticks, must be
        greater than 0. */
        100,
        /* The timers will auto-reload themselves
        when they expire. */
        pdTRUE,
        /* The ID is used to store a count of the
        number of times the timer has expired, which
        is initialised to 0. */
        ( void * ) 3,
        /* Each timer calls the same callback when
        it expires. */
        backlight_timer_callback);
    TimerHandle_t timeout_time = xTimerCreate
      ( /* Just a text name, not used by the RTOS
        kernel. */
        "timeout",
        /* The timer period in ticks, must be
        greater than 0. */
        10000,
        /* The timers will auto-reload themselves
        when they expire. */
        pdFALSE,
        /* The ID is used to store a count of the
        number of times the timer has expired, which
        is initialised to 0. */
        ( void * ) 4,
        /* Each timer calls the same callback when
        it expires. */
        timeout_timer_callback);
    xTimerStart(backlight_time, 0);
    
    vTaskDelay(200);
    for(;;)
    {
        xSemaphoreTake(mutex_handle, 100);
        adc_result = read_adc();
        xSemaphoreGive(mutex_handle);
        if(last_pot == adc_result.pot)
        {
            if(!xTimerIsTimerActive(timeout_time))
            {
                xTimerStart(timeout_time, 0);
            }
        }
        else{
            if(!xTimerIsTimerActive(backlight_time))
            {
                xTimerStart(backlight_time, 0);
            }
            if(xTimerIsTimerActive(timeout_time))
            {
                xTimerStop(timeout_time,0);
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