#include "adc.h"

uint16_t read_adc()
{
    //MUXPOS to AN14 (PF4) for potentiometer
    ADC0.MUXPOS  = ADC_MUXPOS_AIN14_gc;

    ADC0.COMMAND = ADC_STCONV_bm; //Start conversion

    //Wait for hardware to set RESRDY bit
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }

    //return ADC result and clear RSRDY bit
    return ADC0.RES;
}
void adc_init()
{
    
}