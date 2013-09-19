#include "System.h"
#include  <math.h>  
//=====================================================
#ifndef __RC_H
#define __RC_H
//=====================================================

//******************************************************
// NOTE: Functions in this group facilitate reading
//		 input from the RC receiver (special form
//		 of PWM) using the Change Notification (CN) 
//		 feature.
//------------------------------------------------------
// Pulse width is being evaluated using the tick counts
// from the timing module based  upon the 32-bit timer
// comprised of Timer4 and Timer5. It is assumed that
// the timer tick is 0.2 usec.
//------------------------------------------------------
// NOTE: Please initialize TMR component prior to calling
//		 RCInit().
//------------------------------------------------------
// Receiver channels connected to the following pins:
//
// Roll:		Ch1 (CN17/RC7),  -1.0 <= X <= 1.0, Right(+)
// Pitch:		Ch2 (CN20/RC8),  -1.0 <= X <= 1.0, Forward(+) 
// Throttle:	Ch3 (CN19/RC9),   0.0 <= X <= 1.0
// Yaw:			Ch4 (CN16/RB10), -1.0 <= X <= 1.0, Clockwise(+)  
// Control:		Ch5 (CN15/RB11),  X = {0, 1}, 1 -> Switch UP
//
//******************************************************

//----------------------------------------------------------
// Receiver data interface
//----------------------------------------------------------
typedef struct 
	{
	//----------------------
	float		Throttle;	// Ch3 (CN19/RC9)
	//----------------------
	float		Roll;		// Ch1 (CN17/RC7)
	float		Pitch;		// Ch2 (CN20/RC8)
	float		Yaw;		// Ch4 (CN16/RB10)
	//----------------------
	uint		Control;	// Ch5 (CN15/RB11) 
	//----------------------
	} RCData;


//**********************************************************
// RCInit function initializes IC Module.
//**********************************************************
void	RCInit(int IL);	// Parameter (1<=IL<=7) defines the
						// priority of CN interrupt routine.
//----------------------------------------------------------

//**********************************************************
// RCArm function - enforces "arming" of the remote.
// The THROTTLE should go above 75% and then below
// 10% of the total range.
//**********************************************************
void			RCArm();

//**********************************************************
// RCRead function read values from the RC receiver
// in "normalized" form: 
// -1.0 <= X <= 1.0 for Roll, Pitch, and Yaw,
//  0.0 <= X <= 1.0 for Throttle.
//----------------------------------------------------------
// Return value indicates whether a new sample ob-
// tained from the receiver.
//----------------------------------------------------------
// NOTE: Output data structure updated ONLY if a new
//       sample is available!
//**********************************************************
uint		RCRead(RCData*	RCSample);


//**********************************************************
// Same as RCRead(...) function, but will block untill
// the new reading is available from receiver.
//**********************************************************
uint		RCReadWhenReady(RCData*	RCSample);

//**********************************************************
// Returns Connection Status
// 1 - Connection to transmitter is maintained
// 0 - Connection to transmitter is lost
//**********************************************************
uint		RCIsLinked();

//==========================================================
#endif




