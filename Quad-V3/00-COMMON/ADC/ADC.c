#include "ADC\ADCLocal.h"


//************************************************************
uint    ADCGetRawSample()
    {
    return _ADCValue;                
    }
//************************************************************
uint    ADCGetCellCount()
    {
    return _ADCCellCnt;                
    }
//************************************************************
float   ADCGetBatteryNomVoltage()
    {
    return _ADCBatNomV;
    }
//************************************************************
void    ADCGetBatteryStatus(BATData * pStatus)
    {
    //--------------------------------------------------
    // Calculate battery current voltage
    //--------------------------------------------------
    pStatus->BatVoltage = ((float)_ADCValue) * _ADCVRate;
    //--------------------------------------------------
    // Battery charge value (from 0.0 to 1.0 for full charge)
    //--------------------------------------------------
    if (0 == _ADCCellCnt)
        {
        // No external battery... 
        // The board powered from PicKit    
        pStatus->BatCharge = 1.0;    
        }
    else
        {
        if (_ADCValue < _ADCBatMinV)
            pStatus->BatCharge = 0.0;    // Battery is depleted!
        else
            pStatus->BatCharge = ((float)(_ADCValue - _ADCBatMinV))
                               / _ADCBatRange;
        }
    //--------------------------------------------------
    if (pStatus->BatCharge > 1.0)
        pStatus->BatCharge = 1.0;
    //--------------------------------------------------
    return; 
    }
//************************************************************



