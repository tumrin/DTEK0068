/* 
 * File:   adc.h
 * Author: Tuomas Rinne
 *
 * Created on 10 December 2021, 01:52
 */

#ifndef ADC_H
#define	ADC_H
#include <avr/io.h> 

uint16_t read_adc(void);
void adc_init(void);
typedef struct {
    uint16_t ldr;
    uint16_t ntc;
    uint16_t pot;
}ADC_result;



#endif	/* ADC_H */

