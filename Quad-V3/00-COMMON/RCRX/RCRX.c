#include "RCRX/RCRX_Local.h"
//==============================================================================
RCRXStatus	RCRXRead(RCRXData*	RCRXSample)
	{
	//--------------------------------------------------------------------------
    if (0 == _RCRX_IL)
        // RCRX Module is not initialized...
        return RCRX_NotInit;
	//--------------------------------------------------------------------------    
	if (!_RCRX_IsConnected)
        // Connection with RC transmitter either not established yet or lost
		return RCRX_NoConn;		
	//--------------------------------------------------------------------------    
	byte        saved_ipl;     
    RCRXStatus  RCode     = RCRX_OK;
    byte        Frame[_RCRX_FrameSize];
	//--------------------------------------------------------------------------
    ulong       TimeStamp = TMRGetTS();
    float       TimeLapse = (TimeStamp - _RCRX_LastReadTS)*_RCRX_msecRate;          
	//==========================================================================
	// Enter Critical Section...
	//==========================================================================
	SET_AND_SAVE_CPU_IPL(saved_ipl, _RCRX_IL);  // RCRX interrupt is blocked
	//==========================================================================
    if (_RCRX_IsNewFrame)
        // Consume new sample
        {
        bytecopy(Frame, (byte*)_RCRX_RX_NewFrame, _RCRX_FrameSize);
        //----------------------------------------------------------------
        _RCRX_IsNewFrame	= FALSE;		// Sample is consumed...
        }
    else
        // No new frame present... Let's check for how long
        {
        if (TimeLapse >= _RCRX_MaxTimeLapse )
            {
            _RCRX_IsConnected = FALSE;
            // Connection with RC transmitter either not established yet or lost
            RCode = RCRX_NoConn;		            
            }
        else
            // No new reading yet
            RCode = RCRX_NotReady;
        }
	//==========================================================================
	// Leave Critical Section
	//==========================================================================
	RESTORE_CPU_IPL(saved_ipl);
	//==========================================================================
    if (RCRX_OK != RCode)
        // Some error conditions encountered...
        return RCode;
	//==========================================================================
    // The following union assists in converting receiver data stream in
    // "Big Endian" to PIC MCU native integer format in "Little Endian"
    //--------------------------------------------------------------------------
    union
        {
        int              VInt;
        struct
            {
            byte        LowByte;
            byte        HighByte;
            };
       }    U;
	//==========================================================================
    // Throttle, the first channel
    //--------------------------------------------------------------------------
    U.HighByte  = Frame[0];
    U.LowByte   = Frame[1];
    RCRXSample->Throttle = ((U.VInt - 512)/_RCRX_Swing + 1.0)*0.5;
    //-----------------------------------------------------------
    if (RCRXSample->Throttle < 0.0) RCRXSample->Throttle = 0.0;
    if (RCRXSample->Throttle > 1.0) RCRXSample->Throttle = 1.0;
    //--------------------------------------------------------------------------
    // Roll, the second channel (reversed))
    //--------------------------------------------------------------------------
    U.HighByte  = Frame[2];
    U.LowByte   = Frame[3];
    RCRXSample->Roll = -(U.VInt - 1536)/_RCRX_Swing;
    //-----------------------------------------------------------
    if (RCRXSample->Roll < -1.0) RCRXSample->Roll = -1.0;
    if (RCRXSample->Roll >  1.0) RCRXSample->Roll =  1.0;
    //--------------------------------------------------------------------------
    // Pitch, the third channel (reversed)
    //--------------------------------------------------------------------------
    U.HighByte  = Frame[4];
    U.LowByte   = Frame[5];
    RCRXSample->Pitch = -(U.VInt - 2560)/_RCRX_Swing;
    //-----------------------------------------------------------
    if (RCRXSample->Pitch < -1.0) RCRXSample->Pitch = -1.0;
    if (RCRXSample->Pitch >  1.0) RCRXSample->Pitch =  1.0;
    //--------------------------------------------------------------------------
    // Yaw, the fourth channel (reversed)
    //--------------------------------------------------------------------------
    U.HighByte  = Frame[6];
    U.LowByte   = Frame[7];
    RCRXSample->Yaw = -(U.VInt - 3584)/_RCRX_Swing;
    //-----------------------------------------------------------
    if (RCRXSample->Yaw < -1.0) RCRXSample->Yaw = -1.0;
    if (RCRXSample->Yaw >  1.0) RCRXSample->Yaw =  1.0;
    //--------------------------------------------------------------------------
    // Gear, the fifth channel
    //--------------------------------------------------------------------------
    U.HighByte  = Frame[8];
    U.LowByte   = Frame[9];
    if (U.VInt < 4608)
        RCRXSample->Gear = 0;
    else
        RCRXSample->Gear = 1;
    //--------------------------------------------------------------------------
    // Aux1, the sixth channel
    //--------------------------------------------------------------------------
    U.HighByte  = Frame[10];
    U.LowByte   = Frame[11];
    if (U.VInt < 5632)
        RCRXSample->Aux1 = 0;
    else
        RCRXSample->Aux1 = 1;
    //--------------------------------------------------------------------------
    // Finally, set the timestamp of the sample
    //--------------------------------------------------------------------------
    RCRXSample->TS = _RCRX_LastReadTS;
	//==========================================================================
	return RCRX_OK;	// RCSample updated with new sample 
	}
//==============================================================================


//==============================================================================
// RCArm function - enforces "arming" of the remote. The THROTTLE should go
// above 75% and then below 10% of the total range.
//==============================================================================
RCRXStatus	RCRXArm()
	{
	//--------------------------------------------------------------------------
	RCRXData	Capture;
    RCRXStatus  RC;
    //--------------------------------------
    if (0 == _RCRX_IL)
        // RCRX Module is not initialized...
        return RCRX_NotInit;
	//--------------------------------------------------------------------------
WaitForThrottleHigh:
	// Get NEW reading from receiver
    RC = RCRXReadWhenReady(&Capture);
    //--------------------------------------
	if (RCRX_OK !=  RC)
		goto WaitForThrottleHigh;
    //--------------------------------------
	if (Capture.Throttle < 0.75f)	// 75% of the Max Throttle of 1.0	
		goto WaitForThrottleHigh;
	//--------------------------------------------------------------------------
WaitForThrottleLow:
	// Get NEW reading from receiver
    RC = RCRXReadWhenReady(&Capture);
    //--------------------------------------
	if (RCRX_OK !=  RC)             // RCRX_NoConn, for example
		goto WaitForThrottleHigh;   // Something is wrong - start again
    //--------------------------------------
	if (Capture.Throttle > 0.1f)	// 10% of the Max Throttle of 1.0
		goto WaitForThrottleLow;
	//--------------------------------------------------------------------------
	return RCRX_OK;
	}
//==============================================================================


//==============================================================================
// Same as RCRead(...) function, but will block until the new reading is 
// available from receiver or an error condition is raised.
//==============================================================================
RCRXStatus	RCRXReadWhenReady(RCRXData*	RCSample)
	{
    RCRXStatus  RC;
	//--------------------------------------------------------------------------
	while(TRUE)
        {
        if (RCRX_OK == (RC = RCRXRead(RCSample)))
            return RCRX_OK;
        if (RCRX_NotReady != RC) // We are waiting ONLY on RCRX_NotReady
            return RC;
        }
	//--------------------------------------------------------------------------
	}
//==============================================================================
