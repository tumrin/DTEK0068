#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "lcd.h"
#include <stdio.h>
#include "adc.h"
#include "display.h"

void display_task(void *param)
{
    ADC_result adc_results = read_adc();
    
    
    vTaskDelay(200);
    
    for(;;)
    {
        display_ldr(adc_results.ldr);
        display_ldr(adc_results.ldr);
        display_ldr(adc_results.ldr);
    }
    
}
void display_ldr(uint16_t value)
{
    char ldr_string[15];
    snprintf(ldr_string, 15, "LDR value: %d", value);
    lcd_cursor_set(0, 0x05);
    lcd_write(ldr_string);
}
void display_pot(uint16_t value)
{
    char pot_string[15];
    snprintf(pot_string, 15, "LDR value: %d", value);
    lcd_cursor_set(0, 0x05);
    lcd_write(pot_string);
}
void display_ntc(uint16_t value)
{
    char ntc_string[15];
    snprintf(ntc_string, 15, "LDR value: %d", value);
    lcd_cursor_set(0, 0x05);
    lcd_write(ntc_string);
}