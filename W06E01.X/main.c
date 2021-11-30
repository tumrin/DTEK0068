/*
 * File:   main.c
 * Author: Tuomas Rinne
 *
 * Created on 30 November 2021, 12:31
 * Target device: ATmega4809 Curiosity Nano
 */
#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 

// PCn mapping for 7-segment element pin connections
const uint8_t digit[] =
{
    0b00111111,     // 0
    0b00000110,
    0b01011011,
    0b01001111,
    0b01100110,
    0b01101101,     // 5
    0b01111101,
    0b00000111,
    0b01111111,
    0b01101111,      // 9
    0b01110111       // A
};
  
void read_usart(void* param)
{
    
}
void write_usart(void* param)
{
    //Set PA0 to output
    PORTA.DIRSET = PIN0_bm;
}
void control_display(void* param)
{
    // On-board drives the transistor that grounds the 7-segment display
    PORTF.DIRSET = PIN5_bm;
    // Set HIGH to turn off the on-board LED and ground the 7-segment display
    PORTF.OUTSET = PIN5_bm;
    
    // Set entire PORTC (7-segment LED display) as output
    PORTC.DIRSET = 0xFF;
    
    VPORTC.OUT = 0xFF;
    
    vTaskDelete(NULL);
}
  
  
int main(void) 
{ 
    // Create task 
    xTaskCreate( 
        control_display, 
        "display", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    ); 
 
    // Start the scheduler 
    vTaskStartScheduler(); 
 
    // Scheduler will not return 
    return 0; 
}