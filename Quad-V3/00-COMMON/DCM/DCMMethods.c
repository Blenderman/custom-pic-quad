#include "DCM/DCMLocal.h"


//==================================================================================
Matrix*	_DCMNormalize(Matrix* pBase, Matrix* pNorm)
	{
	//============================================================
	// Adjust orthogonality of the rows
	//============================================================
	// Calculate Error based upon the orthogonality of the
	// first two rows:
	float	Error 	= VectorDotProduct(&pBase->Row1, &pBase->Row2);
	//----------------------------------------
	// Apply correction to first two row:
	// Xb = Xb - (Error/2)*Yb;
	// Yb = Yb - (Error/2)*Xb;
	//------------------------------------------------------------
    // Account for the fact that we "subtract" errors
	Error = -Error/2;	
	//------------------------------------------------------------
    // Calculate and store correction vectors in the respective
    // rows of the normalized matrix
	//------------------------------------------------------------
    VectorScale(&pBase->Row2, Error, &pNorm->Row1);
    VectorScale(&pBase->Row1, Error, &pNorm->Row2);
	//------------------------------------------------------------
    // Add rows of the original matrix to correction vectors
	//------------------------------------------------------------
	VectorAdd(&pNorm->Row1, &pBase->Row1, &pNorm->Row1);
	VectorAdd(&pNorm->Row2, &pBase->Row2, &pNorm->Row2);
	//----------------------------------------
	// Third row - a cross-product of the
	// first two: (A=Rx, B=Ry)
	//----------------------------------------
	VectorCrossProduct(&pNorm->Row1, &pNorm->Row2, &pNorm->Row3);
	//-----------------------------------------

	//======================================
	// Normalize the length of axis vectors
	//======================================
	// First row...
	VectorNormalize(&pNorm->Row1, &pNorm->Row1);
	// Second row...
	VectorNormalize(&pNorm->Row2, &pNorm->Row2);
	// Third row...
	VectorNormalize(&pNorm->Row3, &pNorm->Row3);
	//======================================

	return pNorm;
	}
//==================================================================================
