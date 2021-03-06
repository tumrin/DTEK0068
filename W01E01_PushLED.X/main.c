/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: W01E01_PushLED exercise. Lights LED when button is pressed and
 * switches it off when button is released
 *
 * Created on 29 October 2021, 16:57
 */


#include <avr/io.h>

int main(void) 
{
    // Set PF5(LED) as output and PF6(Button) as input
    PORTF.DIR |= PIN5_bm;
    PORTF.DIR &= ~PIN6_bm;

    while (1)
    {
        // If input value of PF6 is high switch LED off else switch it on
        if(PORTF.IN & PIN6_bm)
        {
            PORTF.OUT |= PIN5_bm;
        }
        else
        {
            PORTF.OUT &= ~PIN5_bm;
        }
    }
}
