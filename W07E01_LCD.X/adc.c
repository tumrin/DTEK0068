#include <avr/io.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "adc.h"
#include "usart.h"

/** Function for reading adc
 * 
 * @return 
 */
ADC_result_t read_adc()
{
    xSemaphoreTake(mutex_handle, portMAX_DELAY); // Take mutex lock
    ADC_result_t adc_result;
    
    adc_result.ldr = read_ldr();
    adc_result.ntc = read_ntc();
    adc_result.pot = read_pot();
    
    xSemaphoreGive(mutex_handle); // Release mutex
    return adc_result;
}


uint16_t read_ldr()
{

    ADC0.MUXPOS  = ADC_MUXPOS_AIN8_gc;

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
    ADC0.MUXPOS  = ADC_MUXPOS_AIN9_gc;

    ADC0.COMMAND = ADC_STCONV_bm; //Start conversion

    //Wait for hardware to set RESRDY bit
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm))
    {
        ;
    }

    //return ADC result and clear RSRDY bit
    return ADC0.RES;
}