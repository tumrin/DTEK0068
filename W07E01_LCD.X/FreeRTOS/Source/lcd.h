/* 
 * File:    lcd.h
 *          Minimal 1602 LCD driver (8-bit bus), FreeRTOS version
 * Author:  Jani Tammi <jasata@utu.fi>
 * Created: 13 November 2021, 14:19
 * Device:  ATmega4809 Curiosity Nano
 *
 */

#ifndef LCD_H
#define	LCD_H

#define LCD_LINE0       0
#define LCD_LINE1       1


#ifdef	__cplusplus
extern "C" {
#endif

/*
 * lcd_init()
 *
 *      EXTREMELY SLOW!!! Takes minimum of 103 milliseconds.
 */
void lcd_init(void);

/*
 * lcd_write()
 *
 *      Writes the given string to current cursor position.
 */
void lcd_write(char *str);

/*
 * lcd_cursor_set()
 *
 *      Moves internal cursor to specified position.
 *      x       even = line 0, odd = line 1
 *      y       0x00 ... 0x15, the character position in a line
 */
void lcd_cursor_set(uint8_t x, uint8_t y);

/*
 * lcd_clear()
 *
 *      SLOW! Takes 2 milliseconds. Resets cursor position to (0, 0).
 */
void lcd_clear(void);



#ifdef	__cplusplus
}
#endif

#endif	/* LCD_H */

