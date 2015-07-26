/* 
 * File:   RCRX_Local.h
 * Author: AZarenin
 * Created on July 18, 2015, 11:46 PM
 */
//==============================================================================
#include "RCRX/RCRX.h"
#include "RCRX/RCRX_Profile.h"
//------------------------------------------------------------------------------
#include "TMR/TMR.h"
//==============================================================================
#ifndef RCRX_LOCAL_H
#define	RCRX_LOCAL_H
//==============================================================================
// Some receiver-specific definitions
//------------------------------------------------------------------------------
// Receiver data frame size
#define _RCRX_FrameSize     12   
// Max time (in milliseconds) since last read to indicate connection loss
#define _RCRX_MaxTimeLapse  120.0  
//------------------------------------------------------------------------------
// Variables local to the module; check respective comments in RCRX_Local.c
//------------------------------------------------------------------------------
extern  uint                _RCRX_IL; 
extern  float               _RCRX_msecRate;  
//==============================================================================
extern  byte                _RCRX_Start; 
extern  byte                _RCRX_Stop;  
extern  float               _RCRX_Swing;    // Each channel swing from mid-point
//==============================================================================
// Receiver-specific variables
//------------------------------------------------------------------------------
extern  BOOL    volatile    _RCRX_IsConnected;
extern  BOOL    volatile    _RCRX_IsNewFrame;
extern  ulong   volatile    _RCRX_LastReadTS;
//-----------------------------------------------------
extern  byte 	volatile    _RCRX_RX_NewFrame[_RCRX_FrameSize];
//==============================================================================
// State management variables
//==============================================================================
// State machine
//------------------------------------------------------------------------------
extern  uint    const       _RCRX_RX_StateControl[7][3];
//------------------------------------------------------------------------------
// Receiver Buffer and control variables
//------------------------------------------------------------------------------
extern  uint	volatile	_RCRX_RX_CurrentState;
// Length of the current data in receive buffer
extern  uint	volatile	_RCRX_RX_FrameIdx;
// RCRX receive buffer
extern  byte 	volatile    _RCRX_RX_FrameBuf[_RCRX_FrameSize];
//==============================================================================
// Helper function
//==============================================================================
void static inline bytecopy(byte* Src, byte* Dst, uint Count)
    {
    int i;
    for (i = 0; i < Count; i++)
        Src[i] = Dst[i];
    }
//==============================================================================
#endif	/* RCRX_LOCAL_H */

