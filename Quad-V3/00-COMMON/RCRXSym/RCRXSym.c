/* 
 * File:   RCRX.h
 * Author: AZarenin
 * Created on July 18, 2015, 11:31 PM
 */
//==============================================================================
#include "RCRXSym/RCRXSym_Local.h"
//==============================================================================
// Simulation input data files - ONLY one of the simulation data headers should
// be "uncommented" at a time!
//==============================================================================
//#include "RCRXSym/RCRXSymXData_VibroTest.h"
#include "RCRXSym/RCRXSymXData_BalanceHard.h"
//#include "RCRXSym/RCRXSymXData_BalanceTest.h"
//#include "RCRXSym/RCRXSymXData_MB_Ramp.h"
//#include "RCRXSym/RCRXSymXData_MB_Soft.h"
//#include "RCRXSym/RCRXSymXData_MB_Hard.h"
//#include "RCRXSym/RCRXSymXData_YawTest.h"
//==============================================================================
// RCRXSymArm function - enforces simplified "arming" of the remote  - waiting 
// for Gear channel from receiver to go LOW and then HIGH
//==============================================================================
void	RCRXSymArm()
	{
    //--------------------------------------------------------------------------
    // Make sure that RC receiver is initialized...
    //--------------------------------------------------------------------------
    RCRXInit();
    //--------------------------------------------------------------------------    
	RCRXData		Capture;
    RCRXStatus      RC;
    //--------------------------------------------------------------------------    
RetryLow:
	RCRXReadWhenReady(&Capture);    // Read actual NEW sample
    //--------------------------------------------------------------------------
	if (1 == Capture.Gear)
		goto RetryLow;
    //--------------------------------------------------------------------------
RetryHigh:
	RC = RCRXReadWhenReady(&Capture);   // Read actual NEW sample
    //--------------------------------------------------------------------------
	if (RCRX_NoConn == RC)              // If we loose connection we need
		goto RetryLow;                  // to start again...
    //--------------------------------------------------------------------------
	if (0 == Capture.Gear)
		goto RetryHigh;
    //--------------------------------------------------------------------------
	// Configure local variables
    //--------------------------------------------------------------------------
	_RCRXSym_IsArmed    = TRUE;
    // Set conversion rate from timer ticks to milliseconds
    _RCRXSym_msRate     = TMRGetTSRate() * 1000; 
    // Set simulation start time (in milliseconds)
    _RCRXSym_StartTS    = TMRGetTS() * _RCRXSym_msRate;
    //--------------------------------------------------------------------------
	return;
	}
//==============================================================================

//==============================================================================
// _RCRXSymStop(..) - internal helper function resets receiver data to 0 on
// all channels; Gear = 0 indicates end of simulation cycle
//==============================================================================
static inline RCRXStatus _RCRXSymStop(RCRXStatus RC, RCRXData*	RCRXSample)
    {
    _RCRXSym_IsArmed = FALSE;
    //--------------------------------------
    // Reset output data
    //--------------------------------------
    RCRXSample->Throttle    = 0.0;
    //--------------------------------------
    RCRXSample->Roll        = 0.0;
    RCRXSample->Pitch       = 0.0;
    RCRXSample->Yaw         = 0.0;
    //--------------------------------------	
    RCRXSample->Gear        = 0;
    RCRXSample->Aux1        = 0;
    //--------------------------------------	
    return RC;
    }
//==============================================================================

//==============================================================================
// RCRXSymRead(..) function - reads data from the simulation
// file if RCControl from the receiver is 1
//==============================================================================
RCRXStatus	RCRXSymRead(RCRXData*	RCRXSample)
	{
    //--------------------------------------------------------------------------    
	RCRXData		Capture;
    RCRXStatus      RC;
    //--------------------------------------------------------------------------    
	if(FALSE == _RCRXSym_IsArmed)	
        // Simulation is not ARMed!
		return _RCRXSymStop(RCRX_NotInit, RCRXSample);		
    //--------------------------------------------------------------------------    
	if(FALSE == _RCRX_IsConnected)	
        // Connection to RC receiver is lost - stop the simulation!
		return _RCRXSymStop(RCRX_NoConn, RCRXSample);		
    //-------------------------------------------------------------------------- 
    if (FALSE == _RCRX_IsNewFrame)
        return RCRX_NotReady;
    //--------------------------------------------------------------------------    
	// Read sample from Receiver
    //--------------------------------------------------------------------------    
	RC = RCRXRead(RCRXSample);
    //--------------------------------------------------------------------------    
	if (0 == RCRXSample->Gear)
		// User requested termination of the simulation session
		return _RCRXSymStop(RCRX_NoConn, RCRXSample);				
    //--------------------------------------------------------------------------
    // Number of 20-ms slots since simulation start
    //--------------------------------------------------------------------------
	ulong SymDet =	(TMRGetTS() * _RCRXSym_msRate - _RCRXSym_StartTS) / 20; 												
    //--------------------------------------------------------------------------
	if (SymDet >= _RCRXSymCount)
        // Simulation sequence exhausted!
        return _RCRXSymStop(RCRX_NotInit, RCRXSample); 
    //--------------------------------------------------------------------------
    uint SymIdx = (uint)SymDet;
    //--------------------------------------	
    // Perform simulation step 
    //--------------------------------------
    RCRXSample->Throttle    = ((float)_RCRXSymStream[SymIdx].Throttle)  *0.01;
    //-----------------------
    RCRXSample->Roll		= ((float)_RCRXSymStream[SymIdx].Roll)      *0.01;
    RCRXSample->Pitch		= ((float)_RCRXSymStream[SymIdx].Pitch)     *0.01;
    RCRXSample->Yaw         = ((float)_RCRXSymStream[SymIdx].Yaw)       *0.01;
	//---------------------------------------------------
	return RC;
	}	

								





