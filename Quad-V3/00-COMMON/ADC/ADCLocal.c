#include "ADC\ADCLocal.h"

//==================================================================
uint            _ADCIL          = 4;    // ADC default interrupt level
//------------------------------------------------------------------
ulong volatile  _ADCSampleSum   = 0;    // Accumulator for samples
uint  volatile  _ADCSCnt        = 0;    // Sample count
//==================================================================
// Last (current) ADC measurement expressed in ADC counts.
// "0" indicates absence of a valid measurement
//------------------------------------------------------------------
uint  volatile  _ADCValue       = 0;                                           
//------------------------------------------------------------------
//    Run Variables - set in ADCInit(...)
//------------------------------------------------------------------
// Cell count - established in ADCInit(...) assuming that battery is 
// properly charged before the flight.
uint            _ADCCellCnt     = 0;    
// Battery minimum voltage and battery "useful voltage range" - 
// established in ADCInit(...) based upon the cell count and nominal
// LiPo parameters; expressed in ADC counts:
uint            _ADCBatMinV     = 0;    
uint            _ADCBatRange    = 0;    
// Battery nominal voltage represented in Volts:
float            _ADCBatNomV    = 0.0;    
//==================================================================

//==================================================================
// Specific to the board (depends on the actual values of the voltage
// divider): Conversion rate to obtain real voltage from ADC sample.
//------------------------------------------------------------------
float const        _ADCVRate    = 0.003454;
//------------------------------------------------------------------
// Constants (specific to the board, depend on conversion rate)
// expressed in ADC counts
//------------------------------------------------------------------
// Useful discharge range for 1-cell battery (1.0V: 4.2V -> 3.2V)
uint  const        _ADCRange    = 289;                                        
// Minimum useful voltage for 1-cell battery (3.2V)
uint  const        _ADCMinV     = 926;
// Borderline voltage for 2-cell battery (5.7V)
uint  const        _ADC2CelV    = 1650;                                        
// Borderline voltage for 3-cell battery (8.8V)
uint  const        _ADC3CelV    = 2547;
//==================================================================



