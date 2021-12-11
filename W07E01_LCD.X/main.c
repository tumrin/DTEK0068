/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: Exercise W07E01 - LCD. Program to show number from 0-9
 * on display when read from USART. Displays E and sends error message via USART
 * if character is not a number between 0 and 9.
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
#include "../adc.h"
#include "../usart.h"
#include "../display.h"
#include "../dummy.h"
#include "../display.h"
#include "../backlight.h"
#include "../lcd.h"
  
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
    // Initialization
    ADC0.CTRLA |= ADC_ENABLE_bm;
    init_usart();
    TCB3_init();
    backlight_init();
    //lcd_init();
    
    //output_queue = xQueueCreate(1, sizeof(ADC_result_t));
    
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
  /*      xTaskCreate( 
        dummy_task, 
        "dummy", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    ); 
        xTaskCreate( 
        display_task, 
        "display", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    );*/
 
    // Start the scheduler 
    vTaskStartScheduler(); 
 
    // Scheduler will not return 
    return 0; 
}