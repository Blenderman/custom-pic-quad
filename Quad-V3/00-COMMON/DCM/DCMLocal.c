#include "DCM/DCMLocal.h"

//==================================================================
// Gyro drift cancellation options - all default to TRUE; can be set
// through the call to DCMSetOptions(...)
//==================================================================
BOOL    _DCM_UseAcc = TRUE; // Defines whether to use Acceleremoter
                            // measurements for drift cancellation
BOOL    _DCM_UseMag = TRUE; // Defines whether to use Magnetometer
                            // measurements for drift cancellation
BOOL    _DCM_UseMagYawOnly  = TRUE; 
                            // Force to use Magnetometer ONLY for
                            // compensating Yaw drift
//==================================================================

//==================================================================
// Initialization flag _DCMInit will be set to "1" by DCMReset(...)
//------------------------------------------------------------------
// DCMReset(...) initializes rotation matrix _DCMRM, calculates
// _DCM_BaseAzimuth, sets vector _DCM_BaseMAG (normalized magnetic
// field vector in Earth Frame), and initializes Gyro, Acc, and Mag
// sample timestamps.
//==================================================================
uint	_DCMInit   = 0;  

//==================================================================
// Rotation Matrix - defines rotations experienced by the body
//==================================================================
Matrix	_DCMRM;             // Rotation matrix
//------------------------------------------------------------------
// Normalized Magnetic vector (Earth Frame) captured at the flight
// start position (captured in DCMReset(...) ) provides reference,
// which is being compared to the magnetic vector measured during
// the flight and transformed to Earth frame by current DCM - 
// obviously they should match... :)
//------------------------------------------------------------------
Vector  _DCM_BaseMAG;       
float	_DCM_BaseAzimuth;	// Initial Azimuth
//------------------------------------------------------------------
ulong	_DCM_GyroTS =   0;	// Timestamp of the last Gyro measurement
ulong	_DCM_AccTS  =   0;	// Timestamp of the last Acc measurement
ulong	_DCM_MagTS  =   0;	// Timestamp of the last Mag measurement
//==================================================================



//==================================================================
// Accelerometer could be used to generate correction terms to 
// compensate for the Gyroscope drift (natural and resulting from
// rounding during integration) using PI controller (derivative term
// not used). The following coefficients define P and I terms of the
// correction algorithm:
//------------------------------------------------------------------
const float		_DCMAccKp		= 0.004;		// Kp = 2/tau
const float		_DCMAccKi		= 0.000004;     // Ki = (1/tau)**2
//------------------------------------------------------------------
// Accelerometer-based integral correction term is cumulative by
// nature, so we need to maintain its value
Vector			_DCMAccIterm	= {0.0, 0.0, 0.0};
//==================================================================

//==================================================================
// Magnetometer could be used to generate correction terms to 
// compensate for the Gyroscope drift (natural and resulting from
// rounding during integration) using PI controller (derivative term
// not used). The following coefficients define P and I terms of the
// correction algorithm:
//------------------------------------------------------------------
const float		_DCMMagKp		= 0.004;		// Kp = 2/tau
const float		_DCMMagKi		= 0.000004;		// Ki = (1/tau)**2
//------------------------------------------------------------------
// Magnetometer-based integral correction term is cumulative by
// nature, so we need to maintain its value
//------------------------------------------------------------------
Vector			_DCMMagIterm	= {0.0, 0.0, 0.0};
//==================================================================
