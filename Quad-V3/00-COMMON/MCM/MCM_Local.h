#include "MCM/MCM.h"
#include "MCM/MCM_Profile.h"
//------------------------------------------------------------------------------

//==============================================================================
#ifndef __MCMLocal_H
#define __MCMLocal_H
//==============================================================================
// Indicator of MC initialization
extern uint		_MC_Init;
extern MCMConf  _MC_Config;
//------------------------------------------------------------------------------
// Timer tick count corresponding to PPM pulse width representing "0" Motor 
// Control Signal; pulse of twice this width corresponds to "1" - full power.
extern uint		_MC_Base_Value;
//==============================================================================
// Internal individual Motor Control values containing calculated power output
// settings for each of the four motors
//------------------------------------------------------------------------------
extern float   _MC1;
extern float   _MC2;
extern float   _MC3;
extern float   _MC4;
//==============================================================================
void	_MCM_InitLocal(MCMConf frameConfig, float Period, uint UpTime);
//==============================================================================
#endif





