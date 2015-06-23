#include "ADC\ADCLocal.h"

//************************************************************
void    ADCInit(uint IL)
    {
    //---------------------------------------------------------
    if (IL < 1) IL = 1;
    if (IL > 7) IL = 7;
    _ADCIL    = IL;
    //=========================================================
    // NOTE:    In this application we have only one input to
    //            measure, so we do not need to use DMA.
    //=========================================================
    //    ADC CON1 control register
    //---------------------------------------------------------
    ADCCON1     = 0;        // Disable ADC1, allow reconfiguration
                            // ... unsigned integer format ...
    // ADC1 module enabled in PMD (Peripheral Module Disable)
    // by the common Init(...) function; please see important
    // notes there!
    ADC_12B     = 1;        // 12-bit mode selected
    ADC_SIDL    = 1;        // Discontinue operation in IDLE mode
    ADC_FORM    = 0;        // Conversion result presented as
                            // integer in the range 0 - 4095
    ADC_SSRCG   = 0;        // ADC provides clock source
    ADC_SSRC    = 0b111;    // Internal counter ends sampling and
                            // starts conversion (auto-convert)
    ADC_ASAM    = 1;        // Sampling begins immediately after last
                            // conversion. SAMP bit is auto-set.
    //---------------------------------------------------------

    //=========================================================
    //    ADC CON2 control register
    //---------------------------------------------------------
    ADCCON2     = 0;    // Reference voltage Avdd/Avss
                        // Do NOT scan inputs...
                        // Use only Ch0
    ADC_BUFM    = 1;    // Starts filling the first half of the
                        // buffer on the first interrupt and the
                        // second half of the buffer on the next
                        // interrupt
    ADC_SMPI    = 7;    // Interrupt after every 8th conversion
    // NOTE: There are 16 ADC buffers provided by the MCU; 
    // current configuration (BUFM=1 and SMPI=7) instructs ADC
    // module to fill buffers 0 through 7 (first half) and then
    // issue an interrupt while continuing filling buffers 8
    // through 15. When buffer 15 is filled with the new sample,
    // another interrupt is issues and ADC switches back to 
    // buffers 0 through 7 in the lower half.
    //---------------------------------------------------------

    //=========================================================
    //    ADC CON3 control register
    //---------------------------------------------------------
    // The minimum Tad = 117.6 ns; at this Tad the Tsamp = 3*Tad.
    // Thus the 12-bit conversion will take (3+14)*Tad = 1999.2 ns,
    // which corresponds to nominal speed of 500 kSample/sec.
    //---------------------------------------------------------
    // With the Tcy = 15.625 ns (at 64 mHz), the minimum acceptable
    // Tad will be 125 ns (8*Tcy) with Tsamp = 3*Tad. The maximum
    // conversion speed under these conditions will be:
    // Tconv = (3+14)*Tad = 17*125 = 2,125 ns, which corresponds
    // to the maximum conversion rate of 470.6 kSample/sec.
    //-----------------------------------------------------------
    // Because the sample caps eventually loses charge, conversion
    // rates below 10 kHz may affect linearity performance, 
    // especially at elevated temperatures.
    //-----------------------------------------------------------
    // We are not in a hurry with ADC measurements in this app-
    // lication (battery voltage does not drop "that fast"), so we
    // will set relatively slow conversion rate with the maximum
    // sampling time to account for rather large analog source 
    // impedance (~3.3kOhm) versus recommended 200 Ohm for the fastest
    // conversion rate.
    // Thus we will settle on the following values:
    // Tad   = 160*Tcy  =  2.5 us (ADCS = 159)
    // Tsamp =  26*Tad  = 65.0 us (SAMC = 25)
    // Tconv = (26+14)*Tad = 40*2.5 = 100 us, which results
    // exactly in the minimal recommended conversion rate of
    // 10 KHz.
    //---------------------------------------------------------


    ADCCON3     = 0;
    ADC_ADRC    = 0;    // Clock derived from system clock
    ADC_ADCS    = 159;  // Tad      = (159+1) * Tcy = 2.5 us
    ADC_SAMC    = 25;   // Tsamp = (25+1) * Tad
                                
    //---------------------------------------------------------

    //=========================================================
    //    ADxCON4: ADCx Control Register 4
    //---------------------------------------------------------
    ADCCON4     = 0;    // We are not using DMA...
    //---------------------------------------------------------
        
    
    //=========================================================
    //    ADCCHS123: ADC Input Channel 1, 2, 3 Select Register
    //---------------------------------------------------------
    ADCCHS123   = 0;    // For 12-bit ADC we use only Ch. 0
    //---------------------------------------------------------


    //=========================================================
    //    AD1CHS0: ADC1 Input Channel 0 Select Register
    //---------------------------------------------------------
    ADCCHS0     = 0;        // We will be using only MUX-A
                            // Channel 0 negative input is Vref
    ADC_CH0SA   = ADC_AN;   // Associate Channel 0 positive 
                            // input with defined Analog pin
    //---------------------------------------------------------

    //=========================================================
    //    AD1CSS(H/L): ADC1 Input Scan Select Register
    //---------------------------------------------------------
    AD1CSSH     = 0;    // Disselect all pins from scan
    AD1CSSL     = 0;    // Disselect all pins from scan
    //---------------------------------------------------------

    //=========================================================
    // Configure ADC interrupts
    //---------------------------------------------------------                            
    ADC_IF      = 0 ;       // Clear ADC interrupt flag
    ADC_IE      = 0 ;       // Disable ADC CPU interrupt
    ADC_IP      = _ADCIL;   // Set ADC interrupt priority
    //---------------------------------------------------------
    // After configuration complete enable ADC module
    //=========================================================
    ADC_ON      = 1;
    ADC_IE      = 1 ;       // Enable ADC CPU interrupt
    //=========================================================


    //=========================================================
    // Now we may set up run parameter for current battery
    //=========================================================
    while ( 0 == _ADCValue );       // Wait for first sample...
    //---------------------------------------------------------
    if (_ADCValue > _ADC3CelV)          // V > 8.80v
        _ADCCellCnt = 3;                // 3-cell
    else    if (_ADCValue > _ADC2CelV)  // V > 5.70v
                _ADCCellCnt = 2;        // 2-cell...
            else    
                if (_ADCValue > _ADCMinV)
                    _ADCCellCnt = 1;    // 1-cell   
                else
                    _ADCCellCnt = 0;    // PiC Kit  
    //--------------------------------------
    _ADCBatMinV     = _ADCMinV  * (_ADCCellCnt > 0 ? _ADCCellCnt : 1);
    _ADCBatRange    = _ADCRange * (_ADCCellCnt > 0 ? _ADCCellCnt : 1);
    //--------------------------------------
    _ADCBatNomV     = (_ADCBatMinV + _ADCBatRange) * _ADCVRate;
    //=========================================================
    return;
    }

//************************************************************

