#ifndef __ADCPROFILE_H
#define    __ADCPROFILE_H
//=========================================================================
// On the Quad-V3 boardBattery voltage divider is connected to pin
// AN13/RP9I)45/RB13.
// Also, please NOTE that as we want to perform 12-bit ADC we are limited
// to using ADC1 module ONLY.
//=========================================================================
#define ADC_AN      13
#define ADC_TRIS    _TRISB13
#define ADC_ANS     _ANSB13
//=========================================================================

//=========================================================================
// ADC Control registers and individual control bits
//=========================================================================
// ADC Control register 1
//-------------------------------------------------------------------------
#define ADCCON1     AD1CON1
//-----------------------------------
#define ADC_ON      AD1CON1bits.ADON
#define ADC_SIDL    AD1CON1bits.ADSIDL      // Stop in IDLE mode
#define ADC_12B     AD1CON1bits.AD12B       // 12-bit mode selection
#define ADC_SSRC    AD1CON1bits.SSRC        // Sample Clock Source Select
#define ADC_ASAM    AD1CON1bits.ASAM        // Sampling begins immediately 
#define ADC_DONE    AD1CON1bits.DONE        // ADC Conversion Status 
//-------------------------------------------------------------------------
// ADC Control register 2
//-------------------------------------------------------------------------
#define ADCCON2     AD1CON2
//-------------------------------------------------------------------------
// ADC Control register 3
//-------------------------------------------------------------------------
#define ADCCON3     AD1CON3
#define ADC_ADRC    AD1CON3bits.ADRC    // ADC Conversion Clock Source bit
#define ADC_SAMC    AD1CON3bits.SAMC    // Auto-Sample Time bits
#define ADC_ADCS    AD1CON3bits.ADCS    // ADC Conversion Clock Select bits
                                        // (Defines Tad in reference to Tcy)
//-------------------------------------------------------------------------
// ADC Control register 4
//-------------------------------------------------------------------------
#define ADCCON4     AD1CON4
//-------------------------------------------------------------------------
// ADC Input Channel Selector
//-------------------------------------------------------------------------
#define ADCCHS123   AD1CHS123
//-------------------------------------------------------------------------
// ADC Input Channel 0 Select Register
//-------------------------------------------------------------------------
#define ADCCHS0     AD1CHS0
#define ADC_CH0SA   AD1CHS0bits.CH0SA   // Channel 0 Positive Input Select
                                        // for Sample A bits
//=========================================================================

//=========================================================================
// ADC Result Buffer
//=========================================================================
#define ADCBUF      ADC1BUF0
//=========================================================================

//=========================================================================
// ADC Interrupt control definitions
//=========================================================================
#define ADCInterrupt    _AD1Interrupt
#define ADC_IF          IFS0bits.AD1IF
#define ADC_IE          IEC0bits.AD1IE
#define ADC_IP          IPC3bits.AD1IP

#endif    /* __ADCPROFILE_H */

