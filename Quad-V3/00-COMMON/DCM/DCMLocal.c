#include "DCM/DCMLocal.h"

//==================================================================
// Rotation Matrix - defines rotations experienced by the body
//==================================================================
// _DCMReady will be set to "1" by DCMReset(...), which will also
// initialize rotation matrix _DCMRM, set _DCM_BaseAzimuth and
// _DCM_BaseMAG.
//==================================================================
Matrix	_DCMRM;             // Rotation matrix
//-------------------------------------------------------------
uint	_DCMReady   = 0;    
Vector	_DCM_BaseMAG;       // Initial Magnetic vector (Earth Frame)
float	_DCM_BaseAzimuth;	// Initial magnetic Azimuth
//-------------------------------------------------------------
// DCM Algorith Step Control Variables
//-------------------------------------------------------------
float	_DCM_TimeRate	= 0.0;	// Ticks to second conversion rate
ulong	_DCM_TS 		=   0;	// Timestamp of the last DCM update

//==================================================================
// Accelerometer is used to generate correction terms to adjust
// rotation as measured by Gyroscope using PI(D) algorithm
// (Derivative term is ignored). The following coefficients define
// P and I terms of the correction algorithm:
//==================================================================
const float		_DCMAccKp		= 0.004;		// Kp = 2/tau
const float		_DCMAccKi		= 0.000004;		// Ki = (1/tau)**2


//==================================================================
// Accelerometer-based integral correction term is cumulative by
// nature, so we need to maintain its value
//==================================================================
Vector			_DCMAccIterm	= {0.0, 0.0, 0.0};
