/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: Program to count down from 9 to 0 and display it on 7-segment
 * display
 *
 * Created on 02 November 2021, 00:50
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <avr/delay.h>

uint8_t g_running; //Global variable for stopping timer after interrupt

int main(void) 
{
    //Bitmasks for numbers on display
    uint8_t zero = PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm;
    uint8_t one = PIN2_bm | PIN1_bm;
    uint8_t two = PIN6_bm | PIN4_bm | PIN3_bm | PIN1_bm | PIN0_bm;
    uint8_t three = PIN6_bm| PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm;
    uint8_t four = PIN6_bm | PIN5_bm | PIN2_bm | PIN1_bm;
    uint8_t five = PIN6_bm | PIN5_bm | PIN3_bm | PIN2_bm| PIN0_bm;
    uint8_t six = PIN6_bm | PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN0_bm;
    uint8_t seven = PIN2_bm | PIN1_bm | PIN0_bm;
    uint8_t eight = PIN6_bm | PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN1_bm |
            PIN0_bm;
    uint8_t nine = PIN6_bm | PIN5_bm | PIN2_bm | PIN1_bm | PIN0_bm;
    
    //Put all number on array so we can use counter variable to index them
    uint8_t nums[10] = {zero, one, two, three, four, five, six, seven, eight, 
    nine};
    
    g_running = 1;
    int8_t counter = 9; //Counter for tracking current number
    PORTC.DIRSET = 0xFF; //Set all pins to output
    while (1) 
    {
        if(g_running && counter >= 0)
        {
        //Use VPORTC.OUT because we want to overwrite all bits
        VPORTC.OUT = nums[counter];
            counter--;
        }
        else
        {
            PORTC.OUTTGL = nums[0];
        }
        _delay_ms(1000);
    }
}
