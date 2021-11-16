/*
 * File:   main.c
 * Author: dtek
 *
 * Created on 16 November 2021, 13:16
 */

#include <avr/io.h>
#include <avr/cpufunc.h>
#include <avr/interrupt.h>

#define SERVO_PWM_PERIOD        (0x1046) 
#define SERVO_PWM_DUTY_NEUTRAL  (0x0138)
#define SERVO_PWM_DUTY_MAX (0x01A0)
#define SERVO_PWM_DUTY_MIN (0x00D0)

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

volatile uint8_t g_click = 0;

ISR(RTC_PIT_vect)
{
    RTC.PITINTFLAGS = RTC_PI_bm; //Clear interrupt flags
    if(g_click)
    {
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_MAX; 
        g_click = 0;
    }
    else
    {
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL;     
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
    
}

int main(void) 
{ 
    uint8_t nums[] = {
        ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, A_LETTER
    };
    //Set PORTC pins to output for display
    PORTC.DIRSET = 0xFF;
    
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
    
    
    //Servo
    // Set PE0 as input 
    PORTE.DIRCLR = PIN0_bm; 
    // No pull-up, no invert, disable input buffer 
    PORTE.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc; 
    // Use 2.5v  as reference voltage and set prescaler of 16 
    ADC0.CTRLC |= ADC_PRESC_DIV16_gc; 
    // Enable (power up) ADC (10-bit resolution is default) 
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
    //Potentiometer
    // Set PF4 as input for potentiometer
    PORTF.DIRCLR = PIN4_bm; 
    PORTF.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc; 
    
    VREF.CTRLA |= (0x2<<4); // Set internal voltage ref to 2.5V
    
    rtc_init();
    
    sei(); //Enable interrupts
    
    while (1)
    {
        //Read LDR value
        ADC0.CTRLC &= ~(0x03 << 4); //Clear REFSEL bits
        //Voltage reference to internal 2.5V
        ADC0.CTRLC |= ADC_REFSEL_INTREF_gc;
        //MUXPOS to AN8 (PE0) for LDR
        ADC0.MUXPOS  = ADC_MUXPOS_AIN8_gc;
        // Start conversion (bit cleared when conversion is done) 
        ADC0.COMMAND = ADC_STCONV_bm; 
        // When the conversion is done, the RESRDY bit in the ADC0.INTFLAGS 
        // gets set by the hardware. Without interrupt handler, the program 
        // must wait for that bit to get set before reading the ADC result. 
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) 
        { 
            ;
        } 
        // RESRDY bit must be cleared before another conversion. 
        // Either by writing '1' over it or reading value from ADC0.RES. 
        //ADC0.INTFLAGS = ADC_RESRDY_bm;
        uint16_t res = ADC0.RES/100;
        
        //Read potentiometer value
        //Voltage reference to Vdd
        ADC0.CTRLC &= ~(0x03 << 4); //Clear REFSEL bits
        ADC0.CTRLC |= ADC_REFSEL_VDDREF_gc;
        //MUXPOS to AN14 (PF4) for potentiometer
        ADC0.MUXPOS  = ADC_MUXPOS_AIN14_gc;
        ADC0.COMMAND = ADC_STCONV_bm; 
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) 
        { 
            ;
        } 
        uint16_t treshold = ADC0.RES/100;
        VPORTC.OUT = nums[treshold];
        if((res>=treshold))
        {
            g_click = 1;
                        //Set period to 4096 cycles (1/8 second) and enable PIT function 
            RTC.PITCTRLA = RTC_PERIOD_CYC8192_gc | RTC_PITEN_bm;
            // Enable Periodic Interrupt 
            RTC.PITINTCTRL = RTC_PI_bm;
        }
    }
}
