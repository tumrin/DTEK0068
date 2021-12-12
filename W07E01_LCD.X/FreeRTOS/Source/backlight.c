#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include "timers.h"

ADC_result_t adc_result;

void backlight_timer_callback()
{
    xSemaphoreTake(mutex_handle, 100);
    adc_result = read_adc();
    xSemaphoreGive(mutex_handle);

    uint16_t ratio = adc_result.ldr*10/1023;
    TCB3.CCMP = 0xFFFF - (0xFFFF/10*ratio);
}

void backlight_task(void *param)
{
    TimerHandle_t backlight_time = xTimerCreate
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
        ( void * ) 2,
        /* Each timer calls the same callback when
        it expires. */
        backlight_timer_callback);
    xTimerStart(backlight_time, 0);
    
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