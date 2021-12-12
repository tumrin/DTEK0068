#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "queue.h"
#include <string.h>
#include "usart.h"
#include <stdio.h>
#include "adc.h"

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

/** Initialization for USART
 * 
 */
void init_usart()
{
        //Initialize USART0
    PORTA.DIRSET = PIN0_bm; // Set PA0 to output
    USART0.BAUD = (uint16_t)USART0_BAUD_RATE(9600);
    USART0.CTRLB |= USART_TXEN_bm;
    USART0.CTRLB |= USART_RXEN_bm;
    
}

/** Task for writing to USART
 * 
 */
void write_usart(void* param)
{
    ADC_result_t output_buffer; // Store value from output queue
    char ldr_str[12]; // String to write
    
    vTaskDelay(200);

    for(;;)
    {       xSemaphoreTake(mutex_handle, portMAX_DELAY);
            output_buffer = read_adc();
            xSemaphoreGive(mutex_handle);
            sprintf(ldr_str, "LDR: %d\r\n", output_buffer.ldr);
            USART0_sendString(ldr_str);
    }
    vTaskDelete(NULL);
}