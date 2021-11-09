/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Descriptions: W03E01_BombV2. Enhanced version of previous bomb. Now we use
 * timer instead of delay to decrease timer.
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

volatile uint8_t g_running; //Global variable for stopping timer after interrupt
volatile uint16_t g_clockticks; //Keep track of clockticks for countdown
volatile uint16_t g_pitcount = 0; //Keep track of PIT count

ISR(PORTA_PORT_vect)
{
    VPORTA.INTFLAGS = SET_ALL; //Clear interrupt flags
    g_running = 0;
}
ISR(RTC_PIT_vect)
{
    RTC.PITINTFLAGS = RTC_PI_bm; //Clear interrupt flags
    
    //Increments g_clockticks every 8 PITs
    if(g_pitcount % 8 == 0)
    {
        g_clockticks++;
    }
    g_pitcount++;
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
    //SET_ALL for handling counter == 10 case
    uint8_t nums[11] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, 
                        NINE, SET_ALL};
    
    g_running = 1;
    g_clockticks = 0;
    uint8_t last_tick = 0; //Previous clocktick
    uint8_t counter = 10; //Counter for tracking current number;
    PORTC.DIRSET = SET_ALL; //Set all pins to output
    
    //PA4 rising edge interrupt and enable pull up resistor
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    
    set_sleep_mode(SLEEP_MODE_IDLE); //Sleep mode to IDLE
    
    rtc_init(); //Configure RTC
    sei(); //Enable interrupts
    
    while (1) 
    {
        if(g_running && (g_clockticks != last_tick))
        {
            last_tick = g_clockticks; //Update last_tick
            //Decrease counter before displaying so number won't go down
            //by 1 after disconnecting
            counter--;
            //Use VPORTC.OUT because we want to overwrite all bits
            VPORTC.OUT = nums[counter];
            if(counter == 0)
            {
                g_running = 0;
            }
        }
        else if(counter == 0 && (g_clockticks != last_tick))
        {
            last_tick = g_clockticks;
            PORTC.OUTTGL = nums[counter]; //Display blinking zero
        }
        else
        {
            VPORTC.OUT = nums[counter]; //Display timer when wire was cut
        }
        sleep_mode(); //Set CPU to IDLE state
    }
}
