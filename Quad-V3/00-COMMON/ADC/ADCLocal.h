#include "ADC/ADC.h"
#include "ADC/ADCProfile.h"

//=====================================================
#ifndef __ADCLocal_H
#define __ADCLocal_H
    
//=====================================================
extern uint                 _ADCIL;
//-----------------------------------------------------
extern ulong   volatile     _ADCSampleSum;
extern uint    volatile     _ADCSCnt;
//=====================================================
extern uint    volatile     _ADCValue;
//-----------------------------------------------------
extern uint                 _ADCCellCnt;
//-----------------------------------------------------
extern uint                 _ADCBatMinV;
extern uint                 _ADCBatRange;
//-----------------------------------------------------
extern float                _ADCBatNomV;
//-----------------------------------------------------
// Constants (specific to the board)
//-----------------------------------------------------
extern float   const        _ADCVRate;
//-----------------------------------------------------
extern uint    const        _ADCRange;
extern uint    const        _ADCMinV;
//-----------------------------------------------------
extern uint    const        _ADC2CelV;
extern uint    const        _ADC3CelV;
//=====================================================

#endif



