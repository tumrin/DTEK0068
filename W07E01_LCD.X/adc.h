/* 
 * File:   adc.h
 * Author: Tuomas Rinne
 *
 * Created on 10 December 2021, 01:52
 */

#ifndef ADC_H
#define	ADC_H
#include "FreeRTOS.h"
#include "semphr.h"

// Variables
SemaphoreHandle_t mutex_handle;

// Types
typedef struct {
    uint16_t ldr;
    uint16_t ntc;
    uint16_t pot;
}ADC_result_t;

// Functions
ADC_result_t read_adc();
void adc_init(void);
uint16_t read_ldr(void);
uint16_t read_ntc(void);
uint16_t read_pot(void);

#endif	/* ADC_H */

