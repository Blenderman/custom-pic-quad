#include "MPU\MPU_Local.h"

//==================================================================
// Variables shared (across MPU modules) - set in MPUInit
//==================================================================
byte        _MPU_IL      = 0;    // MPU (INT1) interrupt level
uint        _MPU_Init    = 0;    // Init flag
//==================================================================

//==================================================================
// MPU Calibration options - all default to TRUE; can be set through
// call to MPUSetOptions(...)
//==================================================================
BOOL    _MPU_Rotate = TRUE; // Defines whether to implement axis 
                            // rotation.
BOOL    _MPU_TComp  = TRUE; // Defines whether to compensate for
                            // temperature drift
BOOL    _MPU_Cross  = TRUE; // Defines whether to compensate for
                            // cross-axis disbalance
//==================================================================
void    MPUSetOptions(BOOL Rotate, BOOL TempComp, BOOL CrossAxis)
    {
    _MPU_Rotate = Rotate;
    _MPU_TComp  = TempComp;
    _MPU_Cross  = CrossAxis;
    }
//==================================================================
// <editor-fold defaultstate="collapsed" desc="MPU_CB - MPU Library control block">
//==================================================================
#ifdef Use_MPU1
    MPU_CB            MPU1_CB;
#endif
#ifdef Use_MPU2
    MPU_CB            MPU2_CB;
#endif
//------------------------------------------------------------------
// </editor-fold>
//==================================================================



