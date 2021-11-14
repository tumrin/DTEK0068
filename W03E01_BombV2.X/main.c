/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: W03E01_BombV2. Enhanced version of previous bomb. Now we use
 * timer instead of delay to decrease timer.
 * Note: Wiring is the same as W02E01 and circuit help document with added 
 * transistor and direct GND connections from 7 segment display removed. This is
 * slightly different from W03E01 excercise circuit diagram where GPIO pins are 
 * connected to different pins on 7 segment display.
 *
 * Created on 09 November 2021, 14:18
 */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

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

#define COUNTER_START 9 //Starting point for counter

volatile uint8_t g_running; //Global variable for stopping timer after interrupt
volatile uint8_t g_clockticks; //Keep track of clockticks for countdown

ISR(PORTA_PORT_vect)
{
    VPORTA.INTFLAGS = SET_ALL; //Clear interrupt flags
    g_running = 0;
}

ISR(RTC_PIT_vect)
{
    static uint8_t pit_count = 0; //Keep track of PIT count with static variable
    RTC.PITINTFLAGS = RTC_PI_bm; //Clear interrupt flags
    
    //Stop incrementing if timer should not be running
    if(g_running)
    {
       //Increments g_clockticks every 8 PITs
       //Pitcount is still 7 here on 8th interrupt
       if(pit_count == 7) 
       {
           g_clockticks++;
           pit_count = 0;
       }
       else
       {
           pit_count++;
       }   
    }
}

//Copied from course materials with some modifications
void rtc_init(void)
{
    uint8_t temp;
    
    // Disable oscillator 
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_ENABLE_bm;
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);
    
    // Wait for the clock to be released (0 = unstable, unused) 
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm);
    
    // Select external crystal (SEL = 0) 
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);
    
    // Enable oscillator 
    temp = CLKCTRL.XOSC32KCTRLA;
    temp |= CLKCTRL_ENABLE_bm;
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);
    
    // Wait for the clock to stabilize 
    while (RTC.STATUS > 0);
    
    // Configure RTC module 
    // Select 32.768 kHz external oscillator 
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    
    // Enable Periodic Interrupt 
    RTC.PITINTCTRL = RTC_PI_bm;
    
    //Set period to 4096 cycles (1/8 second) and enable PIT function 
    RTC.PITCTRLA = RTC_PERIOD_CYC4096_gc | RTC_PITEN_bm;
}

int main(void) 
{
    //Put all number on array so we can use counter variable to index them
    uint8_t nums[10] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, 
                        NINE};
    
    g_running = 1;
    g_clockticks = 0;
    
    PORTC.DIRSET = SET_ALL; //Set all pins to output on port C
    
    //PA4 rising edge interrupt and enable pull up resistor
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    
    PORTF.DIRSET = PIN5_bm; //Set PF5 to output
    PORTF.OUTSET = PIN5_bm; //Switch onboard LED off
    
    set_sleep_mode(SLEEP_MODE_IDLE); //Set sleep mode to IDLE
    
    rtc_init(); //Configure RTC
    
    sei(); //Enable interrupts
    
    while (1) 
    {
        if(g_running)
        {
            if(g_clockticks < COUNTER_START)
            {
                //Use VPORTC.OUT because we want to overwrite all bits
                //We can use g_clokticks instead of seperate counter variable
                VPORTC.OUT = nums[COUNTER_START-g_clockticks];
            }
            else
            {
                g_running = 0; //Stop running when timer is zero    
            }
        }
        else if((COUNTER_START-g_clockticks) == 0)
        {
            VPORTC.OUT = nums[COUNTER_START-g_clockticks]; //Display zero
            PORTF.OUTTGL = PIN5_bm; //Blink zero and onboard led
        }
        else
        {
            //Display timer when wire was cut
            VPORTC.OUT = nums[COUNTER_START-g_clockticks];
        }
        sleep_mode(); //Set CPU to IDLE state
    }
}
