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
#include "queue.h"
#define F_CPU 3333333
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * \
(float)BAUD_RATE)) + 0.5)

static QueueHandle_t input_queue;
static QueueHandle_t output_queue;

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
    uint16_t number = 0;
    for(;;)
    {
        vTaskDelay(50);
        while (!(USART0.STATUS & USART_RXCIF_bm))
        {
            ;
        } 
        number = USART0.RXDATAL;
        xQueueSend(input_queue, (void *)&number, 500);
    }
    vTaskDelete(NULL);
}
void write_usart(void* param)
{
    for(;;)
    {
        vTaskDelay(50);
        while (!(USART0.STATUS & USART_DREIF_bm))
        {
            ;
        }        
        //USART0.TXDATAL = 'c';
    }
    vTaskDelete(NULL);
}
void control_display(void* param)
{
    // On-board drives the transistor that grounds the 7-segment display
    PORTF.DIRSET = PIN5_bm;
    // Set HIGH to turn off the on-board LED and ground the 7-segment display
    PORTF.OUTSET = PIN5_bm;
    
    // Set entire PORTC (7-segment LED display) as output
    PORTC.DIRSET = 0xFF;
    
    uint16_t input_buffer;
    
    for(;;)
    {
        vTaskDelay(50);
        if(xQueueReceive(input_queue, &input_buffer, 0) == pdTRUE)
        {
            if(input_buffer < 58 && input_buffer>= 48)
            {
                VPORTC.OUT = digit[input_buffer-48]; 
            }
            else
            {
                VPORTC.OUT = digit[10];
            }
        }       
    }
    
    vTaskDelete(NULL);
}
  
  
int main(void) 
{ 
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
    USART0.CTRLB |= USART_TXEN_bm;
    USART0.CTRLB |= USART_RXEN_bm;
        //Set PA0 to output
    PORTA.DIRSET = PIN0_bm;
    PORTA.DIRCLR = PIN1_bm;
    
    input_queue = xQueueCreate(1, sizeof(uint16_t));
    output_queue = xQueueCreate(1, sizeof(uint16_t));
    // Create task 
    xTaskCreate( 
        control_display, 
        "display", 
        configMINIMAL_STACK_SIZE, 
        NULL, 
        tskIDLE_PRIORITY, 
        NULL 
    ); 
    xTaskCreate( 
        read_usart, 
        "read", 
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
 
    // Start the scheduler 
    vTaskStartScheduler(); 
 
    // Scheduler will not return 
    return 0; 
}