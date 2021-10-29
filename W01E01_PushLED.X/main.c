/*
 * File:   main.c
 * Author: dtek
 *
 * Created on 29 October 2021, 16:57
 */


#include <avr/io.h>

int main(void) {
    PORTF.DIR |= PIN5_bm;
    PORTF.DIR &= ~PIN6_bm;

    while (1)
    {
        if(PORTF.IN & PIN6_bm){
            PORTF.OUT |= PIN5_bm;
        }
        else{
            PORTF.OUT &= ~PIN5_bm;
        }
    }
}
