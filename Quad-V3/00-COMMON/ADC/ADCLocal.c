#include "ADC/ADCLocal.h"

//==================================================================
uint			_ADCIL			= 4;	// ADC default interrupt level
//------------------------------------------------------------------
ulong  volatile	_ADCValue		= 0;	// Last ADC measurement
									// "0" indicates absence of a 
									// valid measurement
//------------------------------------------------------------------
ulong volatile	_ADCBuffer		= 0;	// Accumulator for samples
uint  volatile	_ADCSCnt		= 0;	// Sample count
//==================================================================
//	Run Variables - set in ADCInit(...)
//------------------------------------------------------------------
// These three values are represented in ADC counts:
uint 			_ADCCellCnt		= 0;	
uint 			_ADCBatMinV		= 0;	
uint 			_ADCBatRange	= 0;	
// The following value represented in Volts:
float			_ADCBatNomV		= 0.0;	
//==================================================================

//==================================================================
// Specific to the board (depends on the actual values of the voltage
// divider): Conversion rate to obtain real voltage from ADC sample.
//------------------------------------------------------------------
float const		_ADCVRate		= 0.003872457;
//------------------------------------------------------------------
//	Constants (specific to the board, depend on conversion rate)
//------------------------------------------------------------------
// Minimum useful voltage for 1-cell battery (3.2V)
uint  const		_ADCMinV		= 825;
// Useful discharge range for 1-cell battery (1.0V: 4.2V -> 3.2V)
uint  const		_ADCRange		= 260;										
//==================================================================



