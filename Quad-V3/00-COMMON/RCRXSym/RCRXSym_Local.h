/* 
 * File:   RCRX.h
 * Author: AZarenin
 * Created on July 18, 2015, 11:31 PM
 */
//==============================================================================
#include "RCRXSym/RCRXSym.h"
//------------------------------------------------------------------------------
#include "RCRX/RCRX_Local.h"
//==============================================================================
#ifndef RCSYM_LOCAL_H
#define	RCSYM_LOCAL_H
//==============================================================================

//==============================================================================
// Definitions used internally for simulation
//==============================================================================
// RCSymLine Structure is used internally to represent entries in the 
// Sample Array (see RCRXSymXData_...h)
//------------------------------------------------------------------------------
typedef struct
	{
	sbyte       Throttle;
	//----------------------
	sbyte       Roll;
	sbyte       Pitch;
	sbyte       Yaw;
	} RCSymLine;
//------------------------------------------------------------------------------
// Simulation control values
//------------------------------------------------------------------------------
extern BOOL    _RCRXSym_IsArmed;
extern ulong   _RCRXSym_StartTS;    // Timestamp of simulation start (in msec.)
extern float   _RCRXSym_msRate;     // Conversion rate to convert Timer ticks
                                    // to milliseconds
//------------------------------------------------------------------------------
// Simulation data (defined in the RCRXSymXData_...h file):
//------------------------------------------------------------------------------
extern const uint		_RCRXSymCount;	// Number of entries in simulation file
extern const RCSymLine	_RCRXSymStream[];

//==============================================================================
#endif	/* RCSYM_LOCAL_H */

