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

void display_task(void *param);
void display_ldr(uint16_t value);
void display_pot(uint16_t value);
void display_ntc(uint16_t value);


#ifdef	__cplusplus
}
#endif

#endif	/* DISPLAY_H */

