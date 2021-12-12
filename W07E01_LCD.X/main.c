/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: Exercise W07E01 - LCD. Prgoram to display adc values on lcd
 * display along wit scrolling text. Also has self adjusting backlight.
 * Created on 7 December 2021, 14:37
 * Target device: ATmega4809 Curiosity Nano
 */
#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "semphr.h"
#include "clock_config.h" 
#include "task.h" 
#include "queue.h"
#include <string.h>
#include "adc.h"
#include "usart.h"
#include "display.h"
#include "dummy.h"
#include "display.h"
#include "backlight.h"
#include "lcd.h"
  
void TCB3_init (void)
{
    /* Load CCMP register with the period and duty cycle of the PWM */
    TCB3.CCMP = 0x80FF;

    /* Enable TCB3 and Divide CLK_PER by 2 */
    TCB3.CTRLA |= TCB_ENABLE_bm;
    TCB3.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
    
    /* Enable Pin Output and configure TCB in 8-bit PWM mode */
    TCB3.CTRLB |= TCB_CCMPEN_bm;
    TCB3.CTRLB |= TCB_CNTMODE_PWM8_gc;
}
  
int main(void) // Macro to set baud rate
{
    
    mutex_handle = xSemaphoreCreateMutex(); //Mutex handle for adc
    lcd_queue = xQueueCreate(1, sizeof(ADC_result_t)); // queue for lcd
    
    // Initialization
    VREF.CTRLA |= VREF_ADC0REFSEL_2V5_gc; // Set internal voltage ref to 2.5V
    ADC0.CTRLC &= ~(ADC_REFSEL_VDDREF_gc); //Clear REFSEL bits
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc; //Voltage reference to internal 2.5V
    ADC0.CTRLC |= ADC_PRESC_DIV16_gc; // Set prescaler of 16 
    ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
    
    init_usart();
    TCB3_init();
    backlight_init();
    
    // Create tasks
    xTaskCreate( 
        display_task, 
        "display", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    );
     xTaskCreate( 
        lcd_task, 
        "lcd", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    );    
   xTaskCreate( 
        write_usart, 
        "write", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    ); 
   
        xTaskCreate( 
        backlight_task, 
        "backlight", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    );    
       xTaskCreate( 
        dummy_task, 
        "dummy", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        1, 
        NULL 
    ); 
    // Start the scheduler 
    vTaskStartScheduler(); 
 
    // Scheduler will not return 
    return 0; 
}