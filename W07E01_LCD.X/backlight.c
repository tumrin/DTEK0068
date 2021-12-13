#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"
#include "timers.h"

volatile ADC_result_t adc_result;
volatile uint16_t last_pot;
TimerHandle_t backlight_time;

void backlight_timer_callback()
{
    // Calculate brightness. Default is 0x80FF. Max ldr is 1024 by adding 256
    // We achieve range of 2-128(0x02-0x80) for backlight. Lower byte will stay
    // at 0xFF. Having 2 as minimum ensures that backlight is always on since
    // only time we want it off will be when timout timer is triggered.
    TCB3.CCMP = (((adc_result.ldr+256)/100) << 12) | 0x00FF;
}
void timeout_timer_callback()
{
    if(xTimerIsTimerActive(backlight_time) == pdTRUE)
    {
        xTimerStop(backlight_time, portMAX_DELAY);
    }
    TCB3.CCMP = 0x00FF;
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
    
    xTimerStart(backlight_time, 0); // Start backlight timer
    
    vTaskDelay(200);
    for(;;)
    {
        adc_result = read_adc(); // Read adc
        
        // Check if potentiometer has not been changed. Use 5 as margin of
        // error since pot reading seems to fluctuate a bit.
        if(adc_result.pot < (last_pot + 5) && adc_result.pot > (last_pot - 5))
        {
            // Sart timeout timer
            if(xTimerIsTimerActive(timeout_time) == pdFALSE)
            {
                xTimerStart(timeout_time, portMAX_DELAY);
            }
        }
        else{
            // Save last pot value to compare new reading
            last_pot = adc_result.pot;
            
            // Start backlight timer
            if(xTimerIsTimerActive(backlight_time) == pdFALSE)
            {
                xTimerStart(backlight_time, portMAX_DELAY);
            }
            // Stop timeout timer
            if(xTimerIsTimerActive(timeout_time) == pdTRUE)
            {
                xTimerStop(timeout_time, portMAX_DELAY);
            }
        }
    }
    vTaskDelete(NULL);
}
void backlight_init()
{
    PORTB.DIRSET = PIN5_bm;
    PORTB.OUTSET = PIN5_bm;
}