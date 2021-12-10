/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: Exercise W06E01 - Digit Display. Program to show number from 0-9
 * on display when read from USART. Displays E and sends error message via USART
 * if character is not a number between 0 and 9.
 * Created on 7 December 2021, 14:37
 * Target device: ATmega4809 Curiosity Nano
 */
#include <avr/io.h> 
#include "FreeRTOS.h" 
#include "clock_config.h" 
#include "task.h" 
#include "queue.h"
#include <string.h>
#include "../adc.h"
#include "./../usart.h"

// Macro to set baud rate
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(configCPU_CLOCK_HZ * 64 / (16 * \
(float)BAUD_RATE)) + 0.5)

static QueueHandle_t output_queue;
  
  
int main(void) 
{
    output_queue = xQueueCreate(20, sizeof(uint8_t));
    
    xTaskCreate( 
        write_usart, 
        "write", 
        configMINIMAL_STACK_SIZE, 
        &output_queue, 
        tskIDLE_PRIORITY, 
        NULL 
    ); 
 
    // Start the scheduler 
    vTaskStartScheduler(); 
 
    // Scheduler will not return 
    return 0; 
}