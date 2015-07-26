#include "RCRX/RCRX_Local.h"
//==============================================================================
// RC-RX UART Receive Error Interrupt routine
//------------------------------------------------------------------------------
void __attribute__((__interrupt__,__no_auto_psv__)) RCRXErrInterrupt(void)
	{
	// On error we ignore current frame and reset state automaton
	_RCRX_RX_CurrentState	= 0;	// Reset automaton: Scan input to first START
	//------------------------------
	_RCRX_RX_FrameIdx		= 0;	// Reset index in Receive buffer
	//---------------------------------------------------------------
	// Clear all error conditions
	//---------------------------------------------------------------
	/*
	bit 3 PERR: Parity Error Status bit (Read Only)
		1 = Parity error has been detected for the current character
		0 = Parity error has not been detected
	bit 2 FERR: Framing Error Status bit (Read Only)
		1 = Framing Error has been detected for the current character
		0 = Framing Error has not been detected
	bit 1 OERR: Receive Buffer Overrun Error Status bit (Read/Clear Only)
		1 = Receive buffer has overflowed
		0 = Receive buffer has not overflowed
	*/
	//---------------------------------------------------------------
	UPERR	= 0;
	UFERR	= 0;
	UOERR	= 0;    // Only this error may truly happen :)
	//---------------------------------------------------------------
	UREIF	= 0;    // Clear UART RX Error interrupt flag
	//---------------------------------------------------------------
	return;
	}
//==============================================================================

//==============================================================================
// RC-RX UART Receive Interrupt helper function
//------------------------------------------------------------------------------
void static inline _RCRX_SaveByte(byte	Byte)
	{
	if (_RCRX_RX_FrameIdx < _RCRX_FrameSize)
		{
		_RCRX_RX_FrameBuf[_RCRX_RX_FrameIdx]	= Byte;
		_RCRX_RX_FrameIdx++;
		}
	else
		// Set _RCRX_RX_FrameIdx just above _RCRX_FrameSize
		// to indicate frame overrun;
		// Ignore data bytes beyond _RCRX_FrameSize
		_RCRX_RX_FrameIdx = _RCRX_FrameSize + 1;	
	}
//==============================================================================


//==============================================================================
// RC-RX UART Receive Interrupt routine
//------------------------------------------------------------------------------
void __attribute__((__interrupt__,__no_auto_psv__)) RCRXInterrupt(void)
	{
	uint    Code;
    
	while (URXDA)
		{
		byte	Byte	= URXREG;           // Read received byte
        
        if( Byte == _RCRX_Start )
            Code = 1;                   // 1 - Start code
        else
            if (Byte == _RCRX_Stop )
                Code = 2;               // 2 - Stop code
            else
                Code = 0;               // 0 - Data byte
 		//---------------------------
		// Advance state
		//---------------------------
		_RCRX_RX_CurrentState	= _RCRX_RX_StateControl[_RCRX_RX_CurrentState][Code];
		//--------------------------------
		// Perform Action on received byte
		//--------------------------------
		switch (_RCRX_RX_CurrentState)
			{
			//-------------------------------------------------------------
			case	0:		// 0: Scan to Start - No action...
				break;
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			case	1:		// 1: First Start byte during scan, wait for second...
				break;
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			case	2:		// 2: Second Start byte during scan, ready for frame data...
				break;
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			case	3:		// 3: Data byte received
				_RCRX_SaveByte(Byte);		// Store data byte
				break;
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			case	4:		// 4: Potential "STOP"?
				break;
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			case	5:		// 5: Data after miscellaneous "STOP"
				_RCRX_SaveByte(_RCRX_Stop); // Store skipped STOP
				_RCRX_SaveByte(Byte);		// Store data byte
                //---------------------------------------------------------
				break;
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			case	6:		// 6: Frame terminated
				if ( _RCRX_RX_FrameIdx == _RCRX_FrameSize )
					{
                    // Valid frame... submit frame
                    //----------------------------------------------------------
                    bytecopy(   (byte*)_RCRX_RX_NewFrame, 
                                (byte*)_RCRX_RX_FrameBuf, 
                                _RCRX_FrameSize);
                    //--------------------------------------
                    _RCRX_IsNewFrame    = TRUE;
                    _RCRX_IsConnected   = TRUE;
                    _RCRX_LastReadTS    = TMRGetTS();
                    //----------------------------------------------------------
					}
                //--------------------------------------------------------------
				// Reset receiver
                //--------------------------------------------------------------
				_RCRX_RX_CurrentState	= 0;	// Reset automaton
				_RCRX_RX_FrameIdx		= 0;	// Reset index in Receive buffer
                //--------------------------------------------------------------
				break;
			//-------------------------------------------------------------
			}
		}
	//-------------------------------------------------
	URXIF		= 0; 		// Clear UART RX interrupt flag
	//-------------------------------------------------
	return ;
	}
//==============================================================================
