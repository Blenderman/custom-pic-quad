#include "MCM\MCM_Local.h"
#include <Math.h>

//==============================================================================
void	MCMSet(MCMData*		MCData)
	{
    //--------------------------------------------------------------------------
    // Capture Roll, Pitch, and Yaw adjustment values (as they may need to be
    // changed before applied)
    //--------------------------------------------------------------------------
    float AdjRoll   = MCData->AdjRoll;
    float AdjPitch  = MCData->AdjPitch;
    float AdjYaw    = MCData->AdjYaw;
    //--------------------------------------------------------------------------
    // First, we set all individual motor control values to THROTTLE
    //--------------------------------------------------------------------------
    _MC1 = _MC2 = _MC3 = _MC4 = MCData->Throttle;
    //--------------------------------------------------------------------------
    // Second, we apply Yaw correction; with selected orientation of coordinate 
    // axis (NED) positive Yaw direction is clockwise.
    //      Motors 1 and 3 provide Yaw-positive reaction (motors rotates CCW)
    //      Motors 2 and 4 provide Yaw-negative reaction (motors rotates CW)
    //--------------------------------------------------------------------------
    _MC1 += AdjYaw;
    _MC3 += AdjYaw;
    //-------------------
    _MC2 -= AdjYaw;
    _MC4 -= AdjYaw;
    //--------------------------------------------------------------------------
    // Application of Roll and Pitch motor adjustment depends on the copter
    // configuration - "Plus" or "Cross".
    //--------------------------------------------------------------------------
    // In applying Roll and Pitch adjustment we need to remember that with 
    // selected orientation of coordinate axis (NED) positive Pitch implies
    // that front goes up and positive Roll implies that left side goes up.
    //--------------------------------------------------------------------------
    if (MCM_Plus == _MC_Config)
        // <editor-fold defaultstate="collapsed" desc=""Plus" configuration">
        // "Plus" configuration - Roll and Pitch adjustments applied directly
        {
        //----------------------------------
        // Pitch adjustment
        //----------------------------------
        _MC1 += AdjPitch;       // Front motor
        _MC3 -= AdjPitch;       // Rear motor
        //----------------------------------
        // Roll adjustment
        //----------------------------------
        _MC2 -= AdjRoll;        // Right motor
        _MC4 += AdjRoll;        // Left motor
        }
        // </editor-fold>
    else
        // <editor-fold defaultstate="collapsed" desc=""Cross" configuration">
        // "Cross" configuration - Roll and Pitch adjustments are "split"
        // between motors located at appropriate directions
        {
        // Scale adjustment values by sqrt(2)/2 (0.70710678)
        AdjRoll     = 0.70710678 * AdjRoll;
        AdjPitch    = 0.70710678 * AdjPitch;
        //----------------------------------
        // Pitch adjustment
        //----------------------------------
        _MC1 += AdjPitch;       // Front left motor
        _MC2 += AdjPitch;       // Front right motor
        //---------------
        _MC3 -= AdjPitch;       // Rear right motor
        _MC4 -= AdjPitch;       // Rear left motor
        //----------------------------------
        // Roll adjustment
        //----------------------------------
        _MC1 += AdjRoll;        // Front left motor
        _MC4 += AdjRoll;        // Rear left motor
        //---------------
        _MC2 -= AdjRoll;        // Front right motor
        _MC3 -= AdjRoll;        // Rear right motor 
        }
        // </editor-fold>
    //==========================================================================
	
    //==========================================================================
	// Normalize all calculated motor Throttle values to be >= 0.0
    //--------------------------------------------------------------------------
	float	ThrottleMin		= 0;
    //--------------------------------------------------------------------------
    // Find the smallest negative throttle value 
    //--------------------------------------------------------------------------
	if (_MC1 < ThrottleMin) ThrottleMin = _MC1;
	if (_MC2 < ThrottleMin) ThrottleMin = _MC2;
	if (_MC3 < ThrottleMin) ThrottleMin = _MC3;
	if (_MC4 < ThrottleMin) ThrottleMin = _MC4;
    //--------------------------------------------------------------------------
	if (ThrottleMin < 0)
		{
		//-----------------------------------------------
		_MC1 -= ThrottleMin;
		_MC2 -= ThrottleMin;
		_MC3 -= ThrottleMin;
		_MC4 -= ThrottleMin;
		//-----------------------------------------------
		}
    //==========================================================================

    //==========================================================================
	// Normalize all calculated motor Throttle values to be 0.0 <= X <= 1.0
    //--------------------------------------------------------------------------
	float	ThrottleScale		= 1.0;
	//-----------------------------------------------------------
	if (_MC1 > ThrottleScale) ThrottleScale = _MC1;
	if (_MC2 > ThrottleScale) ThrottleScale = _MC2;
	if (_MC3 > ThrottleScale) ThrottleScale = _MC3;
	if (_MC4 > ThrottleScale) ThrottleScale = _MC4;
	//-----------------------------------------------------------
	if (ThrottleScale > 1.0)
		{
		float KNorm		= 1.0 / ThrottleScale;
		//-----------------------------------
		_MC1 *= KNorm;
		_MC2 *= KNorm;
		_MC3 *= KNorm;
		_MC4 *= KNorm;
		}
    //==========================================================================
    
    //==========================================================================
    // Apply calculated Motor Control values to respective OCx modules
    //==========================================================================
    OC1R = _MC_Base_Value + floorf(_MC_Base_Value * _MC1);
    OC2R = _MC_Base_Value + floorf(_MC_Base_Value * _MC2);
    OC3R = _MC_Base_Value + floorf(_MC_Base_Value * _MC3);
    OC4R = _MC_Base_Value + floorf(_MC_Base_Value * _MC4);
    //==========================================================================
	}
//==============================================================================

