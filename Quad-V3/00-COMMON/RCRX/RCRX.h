/* 
 * File:   RCRX.h
 * Author: AZarenin
 * Created on July 18, 2015, 11:31 PM
 */
//==============================================================================
#include "System.h"
//==============================================================================
#ifndef RCRX_H
#define	RCRX_H
//==============================================================================
typedef enum
    {
    RCRX_OK,
    RCRX_NotInit,       // RCRX Module is not initialized
    RCRX_NotReady,      // No new commands from RC receiver since last call
    RCRX_NoConn         // Connection to transmitter is lost!
    } RCRXStatus;
//------------------------------------------------------------------------------
typedef enum
    {
    LemonRX,
    OrangeRX
    } RCRXFraming;
//------------------------------------------------------------------------------
// Receiver data interface
//------------------------------------------------------------------------------
typedef struct 
	{
	//----------------------
    ulong       TS;
	//----------------------
	float		Throttle;	
	//----------------------
	float		Roll;		
	float		Pitch;		
	float		Yaw;		
	//----------------------
	uint		Gear;	
	uint		Aux1;	
	//----------------------
	} RCRXData;
//******************************************************************************
RCRXStatus  RCRXInit(byte IL, RCRXFraming FrameType);
//******************************************************************************
// RCArm function enforces "arming" of the remote - The THROTTLE should go above
// 75% and then below 10% of the total range.
// NOTE: This call is BLOCKING!!!
//******************************************************************************
RCRXStatus	RCRXArm();
//******************************************************************************
// RCRead function returns values read values from the RC receiver in a
// "normalized" form: 
//      0.0 <= X <= 1.0 for Throttle.
//     -1.0 <= X <= 1.0 for Roll, Pitch, and Yaw 
// For Roll, Pitch, and Yaw sign of the parameter matches direction of
// the respective rotation.
//      X = [0 | 1] for Gear and Aux1.
//------------------------------------------------------------------------------
// NOTE: Output data structure is updated ONLY if a new sample is available!
//******************************************************************************
RCRXStatus	RCRXRead(RCRXData*	RCSample);
//******************************************************************************
// RCReadWhenReady(...) is the same as RCRead(...) function, but will block
// until the new reading is available from receiver or connection is lost.
//------------------------------------------------------------------------------
// Used primarily for testing and simulation, not recommended for control loop.
//******************************************************************************
RCRXStatus	RCRXReadWhenReady(RCRXData*	RCSample);
//==============================================================================
#endif	/* RCRX_H */

