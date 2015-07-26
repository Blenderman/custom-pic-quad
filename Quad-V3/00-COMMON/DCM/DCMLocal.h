#include "DCM/DCM.h"
#include "HMCSPI/HMCSPI.h"

#include "Matrix/Matrix.h"

//=====================================================
#ifndef __DCMLocal_H
#define __DCMLocal_H
//==================================================================
// Gyro drift cancellation options - all default to TRUE; can be set
// through the call to DCMSetOptions(...)
//==================================================================
extern  BOOL    _DCM_UseAcc;
extern  BOOL    _DCM_UseMag;
extern  BOOL    _DCM_UseMagYawOnly; 
//==================================================================

//=====================================================
extern  uint    _DCMInit;
//=====================================================
// DCM is a rotation matrix that converts vectors
// in the Body (moving) frame of reference 
// to the Earth (static) frame of reference.
//=====================================================
// Body axis in the Earth frame of reference are
// the columns of the Rotation Matrix
//----------------------------------------------
// Earth axis in the Body frame of reference are
// the rows of the Rotation Matrix
//=====================================================
//	Xb/e Yb/e Zb/e
//----------------------
//	Rxx, Rxy, Rxz;	// Xe/b - X axis of Earth frame seen from the Body frame
//	Ryx, Ryy, Ryz;	// Ye/b - Y axis of Earth frame seen from the Body frame
//	Rzx, Rzy, Rzz;	// Ze/b - Z axis of Earth frame seen from the Body frame
//-------------------------------------------------------------
extern  Matrix  _DCMRM;				// Rotation matrix
//------------------------------------------------------------------
// Initial Magnetic vector (Earth Frame) captured in DCMReset(...)
extern Vector   _DCM_BaseMAG;       
//-------------------------------------------------------------
extern  float   _DCM_BaseAzimuth;	// Initial Azimuth
//------------------------------------------------------------------
extern  ulong	_DCM_GyroTS;	// Timestamp of the last Gyro measurement
extern  ulong	_DCM_AccTS;     // Timestamp of the last Acc measurement
extern  ulong	_DCM_MagTS;     // Timestamp of the last Mag measurement
//=============================================================

//=============================================================
extern const float	_DCMAccKp;		// Kp = 2/tau
extern const float	_DCMAccKi;		// Ki = (1/tau)**2
//------------------------------------------------------------------
extern 	Vector		_DCMAccIterm;
//=============================================================

//=============================================================
extern const float	_DCMMagKp;		// Kp = 2/tau
extern const float	_DCMMagKi;		// Ki = (1/tau)**2
//------------------------------------------------------------------
extern Vector		_DCMMagIterm;
//==================================================================


//=============================================================
Matrix*	_DCMNormalize(Matrix* pBase, Matrix* pNorm);
//=============================================================

//==========================================================================
// Roll, Pitch, and Yaw of the Body in reference to Earth
// NOTE: We accept the following convention regarding rotation:
//==========================================================================
//	  -Pi <= Roll  <=  +Pi
//-------------------------------------------------------------
float	inline static _DCMRoll(Matrix* pBase)
	{
	if (0 == pBase->Row3.Y && 0 == pBase->Row3.Z )
		return 0.0;	// Special case
	//--------------------------------------------------
	return atan2f(pBase->Row3.Y, pBase->Row3.Z); 
	}
//-------------------------------------------------------------
//	-Pi/2 <= Pitch <= +Pi/2
//-------------------------------------------------------------
float	inline static _DCMPitch(Matrix* pBase)
	{
	if (pBase->Row3.X >=  1.0)	return -1.570796;
	if (pBase->Row3.X <= -1.0)	return +1.570796;
	//--------------------------------------
	return -asinf(pBase->Row3.X);	
	}
//-------------------------------------------------------------
//	  -Pi <= Yaw   <=  +Pi
//-------------------------------------------------------------
float	inline static _DCMYaw(Matrix* pBase)
	{
	if (0 == pBase->Row2.X && 0 == pBase->Row1.X )
		return 0.0;	// Special case
	//--------------------------------------------------
	return atan2f(pBase->Row2.X, pBase->Row1.X); 
	}
//==========================================================================
// Inclination measures Cosine between Ze and Zb;
// For "level" position Incl ~= 1.0 (Cos(0) = 1.0)
// For "upside-down" position Incl ~= -1.0 (Cos(Pi) = -1.0)
//-------------------------------------------------------------
//   -1.0 <= Incl  <= +1.0
//-------------------------------------------------------------
float	inline static _DCMIncl(Matrix* pBase)	
	{ return pBase->Row3.Z; }
//==========================================================================


//==========================================================================
#endif

