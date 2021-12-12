/* 
 * File:   display.h
 * Author: dtek
 *
 * Created on 10 December 2021, 18:02
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#ifdef	__cplusplus
extern "C" {
#endif

// Functions
void display_task(void *param);

// Variables
QueueHandle_t lcd_queue;


#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

