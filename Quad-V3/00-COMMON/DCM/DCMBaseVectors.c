#include "DCM/DCMLocal.h"

//=============================================================
// DCMGetMatrix returns current Rotation Matrix.
// NOTE: rows of the DCM represent Earth frame axis in the
//		 Body frame od reference
//-------------------------------------------------------------
Matrix*		DCMGetMatrix(Matrix* M)
	{
	return MatrixCopy(&_DCMRM, M);
	}
//=============================================================

//=============================================================
// DCMToEarth rotates provided vector from Body frame to Earth
// frame using current DCM matrix
//=============================================================
Vector*		DCMToEarth(Vector* pV, Vector* pRes)
	{
	//--------------------------------------------------
	// Definition of Rotation to Earth:
	//--------------------------------------------------
	// 	Vearth	= R*Vbody
	//--------------------------------------------------
	//	Xe	= XEarth * InBody
	//	Ye	= YEarth * InBody
	//	Ze	= ZEarth * InBody
	//--------------------------------------------------
	return MatrixTimesVector((Matrix*)&_DCMRM, pV, pRes);
	}

//=============================================================
// DCMToBody rotates provided vector from Earth frame to Body
// frame using current DCM matrix
//=============================================================
Vector*		DCMToBody(Vector* pV, Vector* pRes)
	{
	//--------------------------------------------------
	// Definition of Rotation to Body:
	//--------------------------------------------------
	// 	Vbody	= R(Transposed)*Vearth, or
	//	Vbody	= Vearth*R
	//--------------------------------------------------
	//	Xb	= InEarth * XBody
	//	Yb	= InEarth * YBody
	//	Zb	= InEarth * ZBody
	//--------------------------------------------------
	return VectorTimesMatrix(pV, (Matrix*)&_DCMRM, pRes);
	}
//=============================================================

//=============================================================
// DCMEarthX returns X axis of Earth frame as seen from the
// Body frame (roll axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthX(Vector* EarthX)
	{return _DCMEarthX(&_DCMRM, EarthX); }
//=============================================================
// DCMEarthY returns Y axis of Earth frame as seen from the
// Body frame (pitch axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthY(Vector* EarthY)
	{return _DCMEarthY(&_DCMRM, EarthY); }
//=============================================================
// DCMEarthZ returns Z axis of Earth frame as seen from the
// Body frame (yaw axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthZ(Vector* EarthZ)
	{return _DCMEarthZ(&_DCMRM, EarthZ); }
//=============================================================

