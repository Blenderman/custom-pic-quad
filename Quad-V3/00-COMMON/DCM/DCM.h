#include "System.h"
#include "Vector/Vector.h"
#include "Matrix/Matrix.h"

//=============================================================
#ifndef __DCM_H
#define __DCM_H
//=============================================================
#define DCM_OK      0
#define DCM_NOTINIT 1
//=============================================================
// DCMResult - results of DCM calculation
//=============================================================
typedef struct
	{
	ulong	TS;		// Timestamp of the cycle
	//-----------------------------------------------
	// Attitude
	//-----------------------------------------------
	float	Roll;
	float	Pitch;
	float	Yaw;
	//--------------
	float	Incl;
	//--------------
	float	Azimuth;
	//-----------------------------------------------
	}	DCMData;
//=============================================================
// DCMReset resets DCM to initial state and sets drift
// compensation options
//-------------------------------------------------------------
void		DCMReset(BOOL UseAcc, BOOL UseMag, BOOL MagYawOnly,
                     Vector* Acc, Vector* Mag);
//=============================================================
// DCMPerformStep returns value of _DCMReady
//-------------------------------------------------------------
uint		DCMUpdate(	ulong		GyroTS,	// Gyro timestamp
						Vector*		Gyro,   // Gyro measurement
                        //--------------------------------------
                        ulong		AccTS,	// Acc timestamp
						Vector*		Acc,    // Acc measurement
                        //--------------------------------------
                        ulong		MagTS,  // Mag timestamp
						Vector*		Mag,    // Mag measurement
                        //--------------------------------------
						DCMData*	IMUResult);
//=============================================================
// DCMGetMatrix returns current Rotation Matrix.
// NOTE: rows of the DCM represent Earth frame axis in the
//		 Body frame of reference
//-------------------------------------------------------------
Matrix*		DCMGetMatrix(	Matrix*		M);
//=====================================================
Vector*		DCMToEarth(	Vector* pV, Vector* pRes);
Vector*		DCMToBody(	Vector* pV, Vector* pRes);
//==========================================================================
// Body axis in the Earth frame of reference are
// the columns of the Rotation Matrix
//==========================================================================
Vector*     DCMBodyX(Vector* BodyX);
//------------------------------------------------
Vector*     DCMBodyY(Vector* BodyY);
//------------------------------------------------
Vector*     DCMBodyZ(Vector* BodyZ);
//=============================================================
// DCMEarthX returns X axis of Earth frame as seen from the
// Body frame (roll axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthX(Vector* EarthX);
//=============================================================
// DCMEarthY returns Y axis of Earth frame as seen from the
// Body frame (pitch axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthY(Vector* EarthY);
//=============================================================
// DCMEarthZ returns Z axis of Earth frame as seen from the
// Body frame (yaw axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthZ(Vector* EarthZ);
//==========================================================================
// Helper functions (could be used elsewhere as well)
//==========================================================================
// DCM helper routine to normalize Value to 0 <= x <= 2Pi
//--------------------------------------------------------------------------
float inline static DCMRangeTo2Pi(float Value)
	{
	if (Value < 0)
		return (Value + 6.2831853);
	if (Value >  6.2831853)
		return (Value - 6.2831853);
	return Value;
	}
//--------------------------------------------------------------------------
// DCM helper routine to normalize Value to +/- Pi
//--------------------------------------------------------------------------
float inline static DCMRangeToPi(float Value)
	{
	if (Value < -3.14159265)
		return (Value + 6.2831853);
	if (Value >  3.14159265)
		return (Value - 6.2831853);
	return Value;
	}
//--------------------------------------------------------------------------
// DCM helper routine to normalize Value to +/- Pi/2
//--------------------------------------------------------------------------
float inline static DCMRangeToHalfPi(float Value)
	{
	if (Value < -1.57079632)
		return (Value + 3.14159264);
	if (Value >  1.57079632)
		return (Value - 3.14159264);
	return Value;
	}
//==========================================================================
#endif
