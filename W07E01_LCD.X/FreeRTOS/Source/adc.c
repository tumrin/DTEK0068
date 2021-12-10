#include "adc.h"

ADC_result read_adc()
{
    ADC_result adc_result;
    
    adc_result.ldr = read_ldr();
    adc_result.ntc = read_ntc();
    adc_result.pot = read_pot();
    
    return adc_result;
}
uint16_t read_ldr()
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
uint16_t read_pot()
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
uint16_t read_ntc()
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