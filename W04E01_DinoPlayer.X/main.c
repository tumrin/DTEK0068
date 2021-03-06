/*
 * File:   main.c
 * Author: Tuomas Rinne
 * Description: W04E01_DinoPlayer. Program to move servo to click spacebar when
 * photoresistor notices obstacle in game chromium dino game.
 *
 * Created on 16 November 2021, 13:16
 */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

//From course materials
#define SERVO_PWM_PERIOD        (0x1046) 
#define SERVO_PWM_DUTY_NEUTRAL  (0x0138)
#define SERVO_PWM_DUTY_MAX      (0x01A0)
#define SERVO_PWM_DUTY_MIN      (0x00D0)

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
#define A_LETTER PIN6_bm | PIN5_bm | PIN4_bm | PIN2_bm | PIN1_bm | PIN0_bm

//Global variable to keep track when servo should click spacebar
volatile uint8_t g_click = 0;

//Global variable to keep track of returning to neutral position so it doesn't
//get interrupted by new clicking movement
volatile uint8_t g_return = 0;

ISR(RTC_CNT_vect)
{
    RTC.INTFLAGS = RTC_OVF_bm; //Clear interrupt flags
    
    if(!g_return && g_click)
    {
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_MAX; //Press spacebar
        g_click = 0;
        g_return = 1;
        while (RTC.STATUS > 0) //Wait for PERBUSY flag
        {
            ;
        }
        
        //Set period to 100 because we want to return servo to
        //neutral position after pressing spacebar but also want to give
        //movement at least 100ms to complete
        RTC.PER = 100;
    }
    else
    {
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL;
        g_return = 0;
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
    while (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
    {
        ;
    }
    
    // Select external crystal (SEL = 0) 
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);
    
    // Enable oscillator 
    temp = CLKCTRL.XOSC32KCTRLA;
    temp |= CLKCTRL_ENABLE_bm;
    ccp_write_io((void*) &CLKCTRL.XOSC32KCTRLA, temp);
    
    // Wait for the clock to stabilize 
    while (RTC.STATUS > 0)
    {
        ;
    }
    
    // Configure RTC module 
    // Select 32.768 kHz external oscillator 
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;
    RTC.INTCTRL |= RTC_OVF_bm; // Enable OVF interrupt
    
    //Enable RTC and use 32 prescaler so we get convinient 1000 == 1sec for 
    //RTC.PER register values
    RTC.CTRLA = RTC_RTCEN_bm | RTC_PRESCALER_DIV32_gc;
}

/**
 * Function for reading photoresistor values
 * 
 * @return 1/100 of LDR value as 8 bit unsigned integer
 */
uint8_t read_ldr(void)
{
    //Read LDR value
    ADC0.CTRLC &= ~(ADC_REFSEL_VDDREF_gc); //Clear REFSEL bits
    //Voltage reference to internal 2.5V
    ADC0.CTRLC |= ADC_REFSEL_INTREF_gc;
    //MUXPOS to AN8 (PE0) for LDR
    ADC0.MUXPOS  = ADC_MUXPOS_AIN8_gc;
    // Start conversion (bit cleared when conversion is done) 
    ADC0.COMMAND = ADC_STCONV_bm;   
    //Wait for hardware to set RESRDY bit
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }

    //Take 1/100 of result to be easily comparable with threshold
    //and clear RSRDY by reading ADC0.RES
    return ADC0.RES/100;
}

/**
 * Function for reading potentiometer values
 * 
 * @return 1/100 of potentiometer value as 8 bit unsigned integer
 */
uint8_t read_pot(void)
{
    //Read potentiometer value
    ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc; //Voltage reference to Vdd

    //MUXPOS to AN14 (PF4) for potentiometer
    ADC0.MUXPOS  = ADC_MUXPOS_AIN14_gc;

    ADC0.COMMAND = ADC_STCONV_bm; //Start conversion

    //Wait for hardware to set RESRDY bit
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }

    //Take 1/100 of ADC0 result to fit into 7 segment and clear RSRDY bit
    return ADC0.RES/100;
}

int main(void) 
{ 
    uint8_t nums[] = {
        ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, A_LETTER
    };
    //Set PORTC pins to output for display
    PORTC.DIRSET = 0xFF;
    
    //Servo
    // Route TCA0 PWM waveform to PORTB 
    PORTMUX.TCAROUTEA |= PORTMUX_TCA0_PORTB_gc; 
    // Set 0-WO2 (PB2) as digital output 
    PORTB.DIRSET = PIN2_bm; 
    // Set TCA0 prescaler value to 16 (~208 kHz) 
    TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV16_gc; 
    // Set single-slop PWM generation mode 
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_WGMODE_SINGLESLOPE_gc; 
    // Using double-buffering, set PWM period (20 ms) 
    TCA0.SINGLE.PERBUF = SERVO_PWM_PERIOD; 
    // Set initial servo arm position as neutral (0 deg) 
    TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL; 
    // Enable Compare Channel 2 
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP2EN_bm; 
    // Enable TCA0 peripheral 
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
    
    //Photoresistor
    // Set PE0 as input 
    PORTE.DIRCLR = PIN0_bm; 
    // No pull-up, no invert, disable input buffer 
    PORTE.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc; 
    // Set prescaler of 16 
    ADC0.CTRLC |= ADC_PRESC_DIV16_gc; 
    // Enable (power up) ADC (10-bit resolution is default) 
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
    //Potentiometer
    // Set PF4 as input for potentiometer
    PORTF.DIRCLR = PIN4_bm; 
    PORTF.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc; // Disable input buffer
    
    VREF.CTRLA |= VREF_ADC0REFSEL_2V5_gc; // Set internal voltage ref to 2.5V
    
    rtc_init(); //Initialize RCT
    
    //Variables for storing 1/100 of ldr and potentiometer results
    uint8_t ldr_res = 0;
    uint8_t treshold = 0;
    
    sei(); //Enable interrupts
    
    while (1)
    {
        //Read ldr and potentiometer values to variables
        ldr_res = read_ldr();
        treshold = read_pot();
        VPORTC.OUT = nums[treshold]; //Display current threshold
        
        /** Click if photoresistor value is greater or equal to treshold
         *  and servo is not currently returning to neutral position
         *  and servo is not already clicking
         *  This keep tapping as long as ldr value is lower than threshold
         */
        if(ldr_res >= treshold && !g_return && !g_click)
        {
            g_click = 1;
                
            while (RTC.STATUS > 0) // Wait for PERBUSY flag
            {
                ;
            }
            
            /**Set period to 250ms
            *This depends on how far the LDR is from dino meaning
            *how fast should the servo press spacebar when cactus is detected
            *Having at least 100ms here also ensures that servo has
            *at least 100ms to move to neutral position before clicking again
            * */
           RTC.PER = 250;
        }
    }
}
