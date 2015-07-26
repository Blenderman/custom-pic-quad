#include "System.h"
#include <math.h>

#include "Vector\Vector.h"
//=====================================================================
#ifndef __MATRIX_H
#define	__MATRIX_H
//=====================================================================
#define	Deg(x)   ((float)x * 57.29578)
#define Rad(x)	 ((float)x * 0.0174533)
//=====================================================================
typedef struct
	{
    //-----------------------------------------------------------
    // This structure represents 3x3 matrix as a collection
    // of 3 3-dimensional row vectors
    //-----------------------------------------------------------
	Vector  Row1;		// First row
	Vector  Row2;		// Second row
	Vector	Row3;		// Third row
	} Matrix;
//=====================================================================
// <editor-fold defaultstate="collapsed" desc="Column retrieval functions">
static inline Vector* MatrixGetCol1(Matrix* M, Vector* V)
    { return VectorSet(M->Row1.X, M->Row2.X, M->Row3.X, V); }
//---------------------------------------------------------------------
static inline Vector* MatrixGetCol2(Matrix* M, Vector* V)
    { return VectorSet(M->Row1.Y, M->Row2.Y, M->Row3.Y, V); }
//---------------------------------------------------------------------
static inline Vector* MatrixGetCol3(Matrix* M, Vector* V)
    { return VectorSet(M->Row1.Z, M->Row2.Z, M->Row3.Z, V); }
// </editor-fold>
//=====================================================================
// <editor-fold defaultstate="collapsed" desc="Basic matrix operations - Copy, Transpose, Set Identity">
//---------------------------------------------------------------------
static inline Matrix*	MatrixCopy(Matrix* F, Matrix* T)
	{
	VectorCopy(&F->Row1, &T->Row1);
	VectorCopy(&F->Row2, &T->Row2);
	VectorCopy(&F->Row3, &T->Row3);
	//----------------------------------------
	return T;
	}
//---------------------------------------------------------------------
static inline Matrix*	MatrixSetIdentity(Matrix* I)
	{
	VectorSet(1.0, 0.0, 0.0,  &I->Row1);
	VectorSet(0.0, 1.0, 0.0,  &I->Row2);
	VectorSet(0.0, 0.0, 1.0,  &I->Row3);
	//----------------------------------------
	return I;
	}
//---------------------------------------------------------------------
static inline Matrix*	MatrixTranspose(Matrix* M, Matrix* MT)
	{
    MatrixGetCol1(M, &MT->Row1);
    MatrixGetCol2(M, &MT->Row2);
    MatrixGetCol3(M, &MT->Row3);
	//---------------------
	return MT;
	}
//---------------------------------------------------------------------
// </editor-fold>
//=====================================================================
// <editor-fold defaultstate="collapsed" desc="Various multiplication operations">
//---------------------------------------------------------------------
static inline Matrix*	MatrixMult(Matrix* L, Matrix* R, Matrix* T)
	{
	Vector	Col1, Col2, Col3;
	//------------------------------------
	MatrixGetCol1(R, &Col1);
	MatrixGetCol2(R, &Col2);
	MatrixGetCol3(R, &Col3);
	//------------------------------------
    // Set first row of result
	//------------------------------------
	VectorSet(  VectorDotProduct(&L->Row1, &Col1),
                VectorDotProduct(&L->Row1, &Col2),
                VectorDotProduct(&L->Row1, &Col3),
             &T->Row1 );
	//------------------------------------
    // Set second row of result
	//------------------------------------
	VectorSet(  VectorDotProduct(&L->Row2, &Col1),
                VectorDotProduct(&L->Row2, &Col2),
                VectorDotProduct(&L->Row2, &Col3),
             &T->Row2 );
	//------------------------------------
    // Set third row of result
	//------------------------------------
	VectorSet(  VectorDotProduct(&L->Row3, &Col1),
                VectorDotProduct(&L->Row3, &Col2),
                VectorDotProduct(&L->Row3, &Col3),
             &T->Row3 );
	//------------------------------------
	return T;
	}
//---------------------------------------------------------------------
static inline Vector*	MatrixTimesColumnVector(Matrix* L, 
                                                Vector* R, 
                                                Vector* T)
	{
	//------------------------------------
    T->X    = VectorDotProduct(&L->Row1, R);
    T->Y    = VectorDotProduct(&L->Row2, R);
    T->Z    = VectorDotProduct(&L->Row3, R);
	//------------------------------------
	return T;
	}
//---------------------------------------------------------------------
static inline Vector*	RowVectorTimesMatrix(   Vector* L, 
                                                Matrix* R, 
                                                Vector* T)
	{
	Vector	Col;
	//------------------------------------
	T->X	= VectorDotProduct(L, MatrixGetCol1(R, &Col));
	T->Y	= VectorDotProduct(L, MatrixGetCol2(R, &Col));
	T->Z	= VectorDotProduct(L, MatrixGetCol3(R, &Col));
	//------------------------------------
	return T;
	}
//---------------------------------------------------------------------
// </editor-fold>
//=====================================================================
// <editor-fold defaultstate="collapsed" desc="Rotation matrix from Euler angles">
//---------------------------------------------------------------------
// The function MatrixEulerRotation(...) constructs rotation matrix Rm
// in the body frame with respect to the reference frame so that vector
// Vb in the body frame can be converted to the vector Vr in the 
// reference frame by the following matrix multiplication by a column
// vector:
//                  Vr = Rm * Vb.
// The same matrix Rm can be used to perform reverse transformation
// converting Vr into Vb in two ways:
//  1-> By multiplying transpose of Rm by the column vector Vr:
//                  Vb = Rm(T) * Vr, or
//  2-> By multiplying transpose of Vr (row-vector) by matrix Rm:
//                  Vb = Vr(T) * Rm.
//---------------------------------------------------------------------
static inline Matrix*	MatrixEulerRotation(float	Roll,
											float	Pitch,
											float	Yaw,
											Matrix* DCM)
	{
	float	SinRoll		= sinf(Roll);
	float	CosRoll		= cosf(Roll);
	float	SinPitch	= sinf(Pitch);
	float	CosPitch	= cosf(Pitch);
	float	SinYaw		= sinf(Yaw);
	float	CosYaw		= cosf(Yaw);
	//----------------------------------------
	// First Row
	//----------------------------------------
	VectorSet(  CosPitch * CosYaw,
                SinRoll * SinPitch * CosYaw - CosRoll * SinYaw,
                CosRoll * SinPitch * CosYaw + SinRoll * SinYaw,
             &DCM->Row1 );
	//---------------------
	// Second Row
	//----------------------------------------
	VectorSet(  CosPitch * SinYaw,
                SinRoll * SinPitch * SinYaw + CosRoll * CosYaw,
                CosRoll * SinPitch * SinYaw - SinRoll * CosYaw,
             &DCM->Row2 );
	//---------------------
	// Third Row
	//----------------------------------------
	VectorSet( -SinPitch,
                SinRoll * CosPitch,
                CosRoll * CosPitch,
             &DCM->Row3 );
	//----------------------------------------
	return DCM;
	}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// We get infinitesimal rotation matrix from the Euler rotation matrix
// by assuming that for small angles Cos(a) = 1.0, Sin(a) ~ a, and 
// ignoring all terms where we have Sin(...) squared as too small.
//---------------------------------------------------------------------
static inline Matrix*	MatrixSetSmallRotation(Vector* dR, Matrix* DCM)
	{
	// dR (Delta Rotation) - pseudo-vector represents 
    // small rotations: around X (roll), around Y (pitch),
    // and around Z (yaw)
	//--------------------------------------------
	// Infinitesimal Rotation Approximation...
	VectorSet(   1.0,   -dR->Z,     dR->Y,      &DCM->Row1);
	VectorSet( dR->Z,      1.0,    -dR->X,      &DCM->Row2);
	VectorSet(-dR->Y,    dR->X,       1.0,      &DCM->Row3);
	//--------------------------------------------
	//           1.0    -sin(Yaw)   sin(Pitch)
	//       sin(Yaw)       1.0    -sin(Roll)
	//      -sin(Pitch)	 sin(Roll)    1.0
	return DCM;
	}
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Roll, Pitch, and Yaw rotation matrixes we obtain from the Euler
// rotation matrix by assuming that other angles are set to Zero so
// we may substitute in the Euler formula Cos(0)=1.0 and Sin(0) = 0.0
// for all angles except the specified one.
//---------------------------------------------------------------------
static inline Matrix*	MatrixRollRotation(float Roll, Matrix* DCM)
	{
	float	SinRoll		= sinf(Roll);
	float	CosRoll		= cosf(Roll);
	//----------------------------------------
	VectorSet(  1.0,    0.0,        0.0,        &DCM->Row1 );
	VectorSet(  0.0,    CosRoll,   -SinRoll,    &DCM->Row2 );
	VectorSet(  0.0,    SinRoll,    CosRoll,    &DCM->Row3 );
	//----------------------------------------
	return DCM;
	}
//---------------------------------------------------------------------
static inline Matrix*	MatrixPitchRotation(float Pitch, Matrix* DCM)
	{
	float	SinPitch	= sinf(Pitch);
	float	CosPitch	= cosf(Pitch);
	//----------------------------------------
	VectorSet(  CosPitch,   0.0,    SinPitch,   &DCM->Row1 );
	VectorSet(  0.0,        1.0,    0.0,        &DCM->Row2 );
	VectorSet( -SinPitch,   0.0,    CosPitch,   &DCM->Row3 );
	//----------------------------------------
	return DCM;
	}
//---------------------------------------------------------------------
static inline Matrix*	MatrixYawRotation(float	Yaw, Matrix* DCM)
	{
	float	SinYaw		= sinf(Yaw);
	float	CosYaw		= cosf(Yaw);
	//----------------------------------------
	VectorSet(  CosYaw,    -SinYaw, 0.0,        &DCM->Row1 );
	VectorSet(  SinYaw,     CosYaw, 0.0,        &DCM->Row2 );
	VectorSet(  0.0,        0.0,    1.0,        &DCM->Row3 );
	//----------------------------------------
	return DCM;
	}
//---------------------------------------------------------------------
// </editor-fold>
//=====================================================================
#endif	/* __MATRIX_H */

