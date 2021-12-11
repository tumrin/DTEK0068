/* 
 * File:   usart.h
 * Author: dtek
 *
 * Created on 10 December 2021, 01:58
 */

#ifndef USART_H
#define	USART_H

// Macro to set baud rate
#define USART0_BAUD_RATE(BAUD_RATE) ((float)(configCPU_CLOCK_HZ * 64 / (16 * \
(float)BAUD_RATE)) + 0.5)

static QueueHandle_t output_queue;


// Functions
void USART0_sendString(char *str);

// Tasks
void write_usart(void* param);

// Init
void init_usart(void);

#endif	/* USART_H */

