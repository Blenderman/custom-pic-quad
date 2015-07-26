#include "ADC\ADCLocal.h"


//************************************************************
void __attribute__((interrupt, no_auto_psv)) _AD1Interrupt(void)
    {
    ADC_IF      = 0;        // Clear ADC interrupt flag
    //---------------------------------------------------
    // Please recall that with the current setting of
    // BUFM and SMPI an interrupt happens after 8 samples
    // are collected in either the lower or higher half
    // of the buffer
    //---------------------------------------------------
    // Retrieve 8 samples
    //---------------------------------------------------
    if (0 == ADC_BUFS)
        {
        // ADC is filling the lower half, collected samples
        // are in the higher half (buffers 8 through 15).
        // We proceed adding collected samples to the
        // accumulator:
        _ADCSampleSum    += ADC1BUF8;    
        _ADCSampleSum    += ADC1BUF9;    
        _ADCSampleSum    += ADC1BUFA;    
        _ADCSampleSum    += ADC1BUFB;    
        _ADCSampleSum    += ADC1BUFC;    
        _ADCSampleSum    += ADC1BUFD;    
        _ADCSampleSum    += ADC1BUFE;    
        _ADCSampleSum    += ADC1BUFF;    
        }
    else
        {
        // ADC is filling the higher half, collected samples
        // are in the lower half (buffers 0 through 7).
        // We proceed adding collected samples to the
        // accumulator:
        _ADCSampleSum    += ADC1BUF0;    
        _ADCSampleSum    += ADC1BUF1;    
        _ADCSampleSum    += ADC1BUF2;    
        _ADCSampleSum    += ADC1BUF3;    
        _ADCSampleSum    += ADC1BUF4;    
        _ADCSampleSum    += ADC1BUF5;    
        _ADCSampleSum    += ADC1BUF6;    
        _ADCSampleSum    += ADC1BUF7;    
        }
    //---------------------------------------------------
    _ADCSCnt    += 8;       // Increment sample count in
                            // buffer
    //---------------------------------------------------
    // Given the configured ADC rate of 10 KHz, the
    // first update to ADCValue takes 25.6 msec with
    // every subsequent update taking 12.8 msec
    //---------------------------------------------------
   if (_ADCSCnt >= 256)    // 256 samples in accumulator
        {
        //----------------------------------------------
        // Calculate new value as average of accumulated
        // samples: Add 128 and divide by 256 - average 
        // with rounding
        //----------------------------------------------
        _ADCValue = (uint)((_ADCSampleSum + 128) >> 8);
       //--------------------------------------------------------
        _ADCSCnt    = 128;  // Half of the series consumed
                            // Buffer halved (with rounding)
        _ADCSampleSum    = (_ADCSampleSum + 1) >> 1;
        }
    //---------------------------------------------------
    //ADC_ON      = 1;        // Re-start ADC
    }
//************************************************************




