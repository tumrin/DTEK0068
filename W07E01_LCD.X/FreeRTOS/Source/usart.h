/* 
 * File:   usart.h
 * Author: dtek
 *
 * Created on 10 December 2021, 01:58
 */

#ifndef USART_H
#define	USART_H

// Functions
void USART0_sendString(char *str);

// Tasks
void write_usart(void* param);

#endif	/* USART_H */

