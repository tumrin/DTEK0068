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
#define ADC_SAMPLEN 7

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
    // Use Vdd as reference voltage and set prescaler of 16 
    ADC0.CTRLC |= ADC_PRESC_DIV16_gc | ADC_REFSEL_VDDREF_gc; 
    // Enable (power up) ADC (10-bit resolution is default) 
    ADC0.CTRLA |= ADC_ENABLE_bm;
    ADC0.SAMPCTRL = ADC_SAMPLEN;
    ADC0.CTRLE |= ADC_INITDLY_DLY16_gc;
    
    //Potentiometer
    // Set PF4 as input for potentiometer
    PORTF.DIRCLR = PIN4_bm; 
    PORTF.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc; 
    
    while (1) 
    {
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
        uint16_t res = ADC0.RES;
        ADC0.MUXPOS  = ADC_MUXPOS_AIN14_gc;
        ADC0.COMMAND = ADC_STCONV_bm; 
        while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) 
        { 
            ;
        } 
        uint16_t trimmer = ADC0.RES;
        VPORTC.OUT = nums[trimmer/100];
        if(res/100>6)
        {
            TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_MAX; 
        }
        _delay_ms(100);
        TCA0.SINGLE.CMP2BUF = SERVO_PWM_DUTY_NEUTRAL; 
        _delay_ms(100);
    }
}
