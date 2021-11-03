/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: W02E01_Bomb. a Program to count down from 9 to 0 and display it 
 * on 7-segment display. When 0, display blinks. If PA4 is disconnected
 * countdown stops and displays current counter value.
 *
 * Created on 02 November 2021, 00:50
 */

#define F_CPU 3333333

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

//Bitmask for setting all bits to 1
#define SET_ALL 0xFF

//Bitmasks for numbers on display
#define ZERO PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm
#define ONE PIN2_bm | PIN1_bm
#define TWO PIN6_bm | PIN4_bm | PIN3_bm | PIN1_bm | PIN0_bm
#define THREE PIN6_bm| PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm
#define FOUR PIN6_bm | PIN5_bm | PIN2_bm | PIN1_bm
#define FIVE PIN6_bm | PIN5_bm | PIN3_bm | PIN2_bm| PIN0_bm
#define SIX PIN6_bm | PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN0_bm
#define SEVEN PIN2_bm | PIN1_bm | PIN0_bm
#define EIGHT PIN6_bm | PIN5_bm | PIN4_bm | PIN3_bm | PIN2_bm | PIN1_bm \
              | PIN0_bm
#define NINE PIN6_bm | PIN5_bm | PIN2_bm | PIN1_bm | PIN0_bm

volatile uint8_t g_running; //Global variable for stopping timer after interrupt
volatile uint8_t g_counter; //Counter for tracking current number

ISR(PORTA_PORT_vect)
{
    VPORTA.INTFLAGS = SET_ALL; //Clear interrupt flags
    g_running = 0;
    
    /* This prevents display from showing garbage if PA4 is disconnected as
     * timer hits 0
     */
    if(g_counter == 0)
    {
        PORTC.OUTCLR = SET_ALL; //Clear display for toggling to work correctly
    }
    PORTA.PIN4CTRL &= ~PORT_ISC_RISING_gc; //Disable interrupt for PA4
}

int main(void) 
{ 
    //Put all number on array so we can use counter variable to index them
    uint8_t nums[10] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, 
                        NINE};
    
    g_running = 1;
    g_counter = 9;
    PORTC.DIRSET = SET_ALL; //Set all pins to output
    PORTA.PIN4CTRL = PORT_ISC_RISING_gc; //PA4 rising edge interrupt
    
    sei(); //Enable interrupts
    
    while (1) 
    {
        if(g_running)
        {
            //Use VPORTC.OUT because we want to overwrite all bits
            VPORTC.OUT = nums[g_counter];
            if(g_counter == 0)
            {
                g_running = 0;
            }
            else{
                g_counter --;
            }
        }
        else if(g_counter == 0)
        {
            PORTC.OUTTGL = nums[g_counter]; //Display blinking zero
        }
        else{
            VPORTC.OUT = nums[g_counter]; //Display timer when wire was cut
        }
        _delay_ms(1000);
    }
}
