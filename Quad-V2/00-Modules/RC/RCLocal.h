#include "RC\RC.h"
#include "TMR\TMRLocal.h"

//=====================================================
#ifndef __RCLocal_H
#define __RCLocal_H
//=====================================================

//-----------------------------------------------------------
// RCTimingData Structure is used internally to represent
// start/end time(s) of receiver pulses for each channel
// NOTE: Typically there is about 70 usec interval between
// 		 the falling edge of one channel and rising edge
//		 of subsequent one; we will adjust for this interval
//		 in the code because (a) it is regular, and (b) it
//		 does not make sense to generate another interrupt
//		 to measure a known value.
//-----------------------------------------------------------
typedef union 
	{
	struct
		{
		long	Ch5Start;
		long	Ch4Start;
		long	Ch3Start;
		long	Ch2Start;
		long	Ch1Start;
		long	Ch1End;
		// Ch6 is not routed on the board so it is being ignored...
		};
	long RCTimingArray[6];
	} RCTimingData;

//-----------------------------------------------------------
// RCRawData Structure is used internally to represent
// normalized ("centered" if necessary and forced into the
// range) RC sample data in "ticks" 
//-----------------------------------------------------------
typedef struct 
	{
	long	Ch1;		// Control
	long	Ch2;		// Yaw
	long	Ch3;		// Pitch
	long	Ch4;		// Roll
	long	Ch5;		// Throttle
	} RCRawData;

//************************************************************
// Definitions used internally in the module
//************************************************************
// Module control values
//------------------------------------------------------------
extern uchar	_RCIL; 		// Default interrupt priority
extern uint		_RCInit;	// Initialization flag

//------------------------------------------------------------
// Timer3 rolover control (assuming Fcy = 40 MHz and Timer3
// prescaler is set to 1:64 for 1 tick = 1.6 usec) for 25 msec:
//------------------------------------------------------------
extern const uint				_RC_Tmr_Count;
//------------------------------------------------------------
// Channel time count range values:
//------------------------------------------------------------
extern const long				_RCTimeCountMinMin;
extern const long				_RCTimeCountMin;
extern const long				_RCTimeCountMidPoint;
extern const long				_RCTimeCountMax;
extern const long				_RCTimeCountMaxMax;

//------------------------------------------------------------
// Channel time count normalization values:
//------------------------------------------------------------
extern const float				_RCPlusRange;
extern const float				_RCPlusMinusRange;

//------------------------------------------------------------
// Control automaton state
//------------------------------------------------------------
extern volatile uint			_RCState;	
extern volatile RCTimingData	_RCTimes;

//------------------------------------------------------------
// Sample Flags:
//------------------------------------------------------------
extern volatile uint		_RCConnActive;	// 1 - last update 
											// was from Receiver
//-------------
extern volatile uint		_RCSampleReady;	// 1 - new sample 
											// was loaded
extern volatile uint		_RCSampleTimed;	// 1 - new sample
											// emulated by timer
//------------------------------------------------------------
// RCSample->Control is very critical for operation as when
// this input is set to 0 the code may cut off the engines,
// so it is critical to provide some "safety margin" to filter
// out some random flukes thus Control is set to 0 only if 
// _RCControlMin samples in a row give 0 control value
//------------------------------------------------------------
extern const	uint		_RCControlMin;
//------------------------------------------------------------
extern volatile RCRawData	_RCRawSample;	
//=====================================================
#endif





