#include "DCM/DCMLocal.h"
#include "TMR/TMR.h"

#include <math.h>

//==============================================================================
void		DCMReset(BOOL UseAcc, BOOL UseMag, BOOL MagYawOnly,
                     Vector* Acc, Vector* Mag)
	{
    // First, some "housekeeping" ...
    TMRInitDefault();	// Most probably timer is already initialized
                        // so this call is just a NOP
	//------------------------------------------------------------------
    // Set gyro drift compensation options
	//------------------------------------------------------------------
    _DCM_UseAcc         = UseAcc;
    _DCM_UseMag         = UseMag;
    _DCM_UseMagYawOnly  = MagYawOnly;

	//------------------------------------------------------------------
	// Hopefully the Acceleration vector A we received was averaged over
    // some time (maybe, 200-300 msec) and is rather stable as we will
    // derive from it initial orientation of the quad...
	//------------------------------------------------------------------
    // Please NOTE: As we have Axis Z pointing down and gravity 
    // acceleration vector pointing up (-1.0), we have to change sign
    // on arguments of the Atan2 function to get the correct quadrant
	float   Roll    = atan2f(-Acc->Y, -Acc->Z);
    //
    float   Pitch   = atan2f(Acc->X, sqrtf(Acc->Y*Acc->Y + Acc->Z*Acc->Z));
    float   Yaw     = 0.0;
    // Using calculated values for Roll, Pitch, and Yaw we may build
    // initial Rotation Matrix
    MatrixEulerRotation(Roll, Pitch, Yaw, &_DCMRM);
	//------------------------------------------------------------------

	//------------------------------------------------------------------
	// Now that we established rotation matrix, we may convert magnetic
    // vector measured in Body frame to Earth frame, normalize (as we do
    // not care about the strength of magnetic field, just its direction)
    // and store for future use...
	//------------------------------------------------------------------
	DCMToEarth(Mag, &_DCM_BaseMAG);
    VectorNormalize(&_DCM_BaseMAG, &_DCM_BaseMAG);
	//-----------------------------------------------
	// ...and then calculate Base Azimuth
	//-----------------------------------------------
	if (0 != _DCM_BaseMAG.X || 0 != _DCM_BaseMAG.Y )
		_DCM_BaseAzimuth = atan2f(-_DCM_BaseMAG.Y, _DCM_BaseMAG.X);
	else
		_DCM_BaseAzimuth = 0.0;
	//------------------------------------------------------------------
	// If magnetic vector is used ONLY for Yaw drift compensation we 
    // should nullify Z-axis component
	//------------------------------------------------------------------
    if (TRUE == _DCM_UseMag && TRUE == _DCM_UseMagYawOnly)
        _DCM_BaseMAG.Z = 0.0;    
    
	//==================================================================
	// Accelerometer-based and Magnetometer-based integral correction 
    // terms are cumulative by nature, so we need to reset their values
	//==================================================================
	VectorSet(0.0, 0.0, 0.0, &_DCMAccIterm);
	VectorSet(0.0, 0.0, 0.0, &_DCMMagIterm);
	//==================================================================
	_DCMInit		=   1;          // DCM initialization is successful
    // Timestamps of DCM initialization - later used as the last timestamp
    // of respective sensor measurements
	_DCM_GyroTS = _DCM_AccTS = _DCM_MagTS = TMRGetTS();   
	}
//==================================================================================



