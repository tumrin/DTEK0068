/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: Exercise W06E01 - Digit Display. Program to show number from 0-9
 * on display when read from USART. Displays E and sends error message via USART
 * if character is not a number between 0 and 9.
 * Created on 30 November 2021, 12:31
 * Target device: ATmega4809 Curiosity Nano
 */
#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "queue.h"
#include <string.h>

// Macro to set baud rate
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(configCPU_CLOCK_HZ * 64 / (16 * \
(float)BAUD_RATE)) + 0.5)

// Queues
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
    0b01111001       // E
};

/** Function to send string via USART0
 * 
 * @param str Pointer to string
 */
void USART0_sendString(char *str)
{
    for(size_t i = 0; i < strlen(str); i++)
    {
        while (!(USART0.STATUS & USART_DREIF_bm))
        {
            ;
        }        
        USART0.TXDATAL = str[i];
    }
}

void read_usart(void* param)
{
    PORTA.DIRCLR = PIN1_bm; // Set PA1 to input
    uint8_t number = 0; // Variable for storing number from user input
    
    for(;;)
    {
        while (!(USART0.STATUS & USART_RXCIF_bm))
        {
            ;
        } 
        number = USART0.RXDATAL; // Read number from USART0
        
        // Check if number is between 0 and 9. '0' = 48 and '9' = 57 in ASCII
        if((number >= 48) || (number <= 57))
        {
            number -= 48; // Convert ASCII number to integer
        }
        
        // Send received number to both queues
        xQueueSend(input_queue, (void *)&number, 0);
        xQueueSend(output_queue, (void *)&number, 0);
    }
    vTaskDelete(NULL);
}

void write_usart(void* param)
{
    PORTA.DIRSET = PIN0_bm; // Set PA0 to output

    uint8_t output_buffer; // Store value from output queue

    for(;;)
    {
        while (!(USART0.STATUS & USART_DREIF_bm))
        {
            ;
        }      
        
        // Send error if number in queue is more than 9
        if(xQueueReceive(output_queue, &output_buffer, 0) == pdTRUE)
        {
            if(output_buffer > 9)
            {
                USART0_sendString("Error! Not a valid digit.\r\n");
            }
            else
            {
                USART0_sendString("Correct! This is a digit.\r\n");
            }
        }
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
    
    uint8_t input_buffer; // Buffer for receiving number from read task
    
    for(;;)
    {
        if(xQueueReceive(input_queue, &input_buffer, 0) == pdTRUE)
        {
            if((input_buffer <= 9) && (input_buffer >= 0))
            {
                VPORTC.OUT = digit[input_buffer];
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
    //Initialize USART0
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
    USART0.CTRLB |= USART_TXEN_bm;
    USART0.CTRLB |= USART_RXEN_bm;
    
    //Initialize queues
    input_queue = xQueueCreate(1, sizeof(uint8_t));
    output_queue = xQueueCreate(1, sizeof(uint8_t));
    
    // Create tasks
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