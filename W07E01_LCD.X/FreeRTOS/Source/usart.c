#include "usart.h"
#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "queue.h"
#include <string.h>

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

void init_usart()
{
        //Initialize USART0
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
    USART0.CTRLB |= USART_TXEN_bm;
    USART0.CTRLB |= USART_RXEN_bm;
    
}

void write_usart(void* param)
{
    PORTA.DIRSET = PIN0_bm; // Set PA0 to output

    uint8_t output_buffer; // Store value from output queue
    
    vTaskDelay(200);

    for(;;)
    {        
        // Send error if number in queue is more than 9
        if(xQueueReceive(param, &output_buffer, 0) == pdTRUE)
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