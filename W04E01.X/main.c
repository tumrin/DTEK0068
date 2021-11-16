/*
 * File:   main.c
 * Author: dtek
 *
 * Created on 16 November 2021, 13:16
 */

#define F_CPU 333333
#include <avr/io.h>
#include <util/delay.h>
#define SERVO_PWM_PERIOD        (0x1046) 
#define SERVO_PWM_DUTY_NEUTRAL  (0x0138)
#define SERVO_PWM_DUTY_MAX (0x01A0)
#define SERVO_PWM_DUTY_MIN (0x00D0)

int main(void) 
{ 
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
    
    // ADC input pin must be: 
    //    1. Configured as input 
    //    2. Have pull-up disabled (this is start-up default setting) 
    //    3. Have digital input buffer disabled 
    // Using AN0 (PD0) (default in ADC0.MUXPOS)   
    // Set PD0 as input 
    PORTD.DIRCLR = PIN0_bm; 
    // No pull-up, no invert, disable input buffer 
    PORTD.PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc; 
    // Use Vdd as reference voltage and set prescaler of 16 
    ADC0.CTRLC |= ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc; 
    // Enable (power up) ADC (10-bit resolution is default) 
    ADC0.CTRLA |= ADC_ENABLE_bm;
    
    while (1) 
    {
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
         ADC0.INTFLAGS = ADC_RESRDY_bm; 
         
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_MIN; 
        _delay_ms(1000);
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL; 
    }  
}
