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
// DCMToEarth transforms vector from Body frame to Earth
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
	return MatrixTimesColumnVector(&_DCMRM, pV, pRes);
	}

//=============================================================
// DCMToBody transform vector from Earth frame to Body
// frame using current DCM matrix
//=============================================================
Vector*		DCMToBody(Vector* pV, Vector* pRes)
	{
	//--------------------------------------------------
	// Definition of Rotation to Body:
	//--------------------------------------------------
	// 	Vbody(column)	= R(Transposed)*Vearth(column)
    //
    // If we transpose both sides of the formula above,
    // we get the following equation:
    //
	//	Vbody(row)	= Vearth(row)*R
    // 
    // which allows us to avoid transposing matrix.
	//--------------------------------------------------
	//	Xb	= InEarth * XBody
	//	Yb	= InEarth * YBody
	//	Zb	= InEarth * ZBody
	//--------------------------------------------------
	return RowVectorTimesMatrix(pV, &_DCMRM, pRes);
	}
//=============================================================

//==========================================================================
// Body axis in the Earth frame of reference are
// the columns of the Rotation Matrix
//==========================================================================
Vector*     DCMBodyX(Vector* BodyX)
	{return MatrixGetCol1(&_DCMRM, BodyX); }
//------------------------------------------------
Vector*     DCMBodyY(Vector* BodyY)
	{return MatrixGetCol2(&_DCMRM, BodyY); }
//------------------------------------------------
Vector*     DCMBodyZ(Vector* BodyZ)
	{return MatrixGetCol3(&_DCMRM, BodyZ); }
//==========================================================================

//=============================================================
// Earth axis in the Body frame of reference are the rows of
// the Rotation Matrix
//=============================================================
// DCMEarthX returns X axis of Earth frame as seen from the
// Body frame (roll axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthX(Vector* EarthX)
	{return VectorCopy(&_DCMRM.Row1, EarthX); }
//=============================================================
// DCMEarthY returns Y axis of Earth frame as seen from the
// Body frame (pitch axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthY(Vector* EarthY)
	{return VectorCopy(&_DCMRM.Row2, EarthY); }
//=============================================================
// DCMEarthZ returns Z axis of Earth frame as seen from the
// Body frame (yaw axis) from the current DCM matrix
//-------------------------------------------------------------
Vector*		DCMEarthZ(Vector* EarthZ)
	{return VectorCopy(&_DCMRM.Row3, EarthZ); }
//=============================================================

