#include "System.h"

//=====================================================
#ifndef __ADC_H
#define __ADC_H
//=====================================================

//=====================================================
// ADC subroutines in this program use AN1
//=====================================================
// Parameter (1<=IL<=7) defines the priority of ADC
// interrupt routine.
//-----------------------------------------------------
void    ADCInit(uint IL);                               
//-----------------------------------------------------
// Utility function 
//-----------------------------------------------------
// Last measured battery voltage in ADC counts.
uint    ADCGetRawSample();
// Battery cell count
uint    ADCGetCellCount();
// Battery nominal max voltage
float   ADCGetBatteryNomVoltage();
//-----------------------------------------------------
typedef struct
    {
    // Charge level in %
    float   BatCharge;
    // Battery current voltage
    float   BatVoltage;
    } BATData;
//-----------------------------------------------------
void    ADCGetBatteryStatus(BATData * pStatus);
//=====================================================
#endif



