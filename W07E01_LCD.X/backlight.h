/* 
 * File:   backlight.h
 * Author: dtek
 *
 * Created on 10 December 2021, 23:16
 */

#ifndef BACKLIGHT_H
#define	BACKLIGHT_H

#ifdef	__cplusplus
extern "C" {
#endif

// Functions
void backlight_task(void *param);
void backlight_init(void);


#ifdef	__cplusplus
}
#endif

#endif	/* BACKLIGHT_H */

