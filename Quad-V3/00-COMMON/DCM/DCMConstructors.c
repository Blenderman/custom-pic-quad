#include "DCM/DCMLocal.h"

//==================================================================================
Matrix*	_DCMCopy(Matrix* pBaseDCM, Matrix* pNewDCM)
	{
	//------------------------------------------------------------------------------
	pNewDCM->Rxx = pBaseDCM->Rxx;	pNewDCM->Rxy = pBaseDCM->Rxy;	pNewDCM->Rxz = pBaseDCM->Rxz;
	pNewDCM->Ryx = pBaseDCM->Ryx;	pNewDCM->Ryy = pBaseDCM->Ryy;	pNewDCM->Ryz = pBaseDCM->Ryz;
	pNewDCM->Rzx = pBaseDCM->Rzx;	pNewDCM->Rzy = pBaseDCM->Rzy;	pNewDCM->Rzz = pBaseDCM->Rzz;
	//------------------------------------------------------------------------------
	return pNewDCM;
	}
//==================================================================================
Matrix*	_DCMSmallRotation(Vector* pDR, Matrix* pNewDCM)
	{
	pNewDCM->Rxx	=  1.0F;	pNewDCM->Rxy = ;		pNewDCM->Rxz =  ;
	pNewDCM->Ryx =  ;		pNewDCM->Ryy =  1.0F;		pNewDCM->Ryz = ;
	pNewDCM->Rzx = ;		pNewDCM->Rzy =  ;		pNewDCM->Rzz =  1.0F;
	return pNewDCM;
	}
//==================================================================================

