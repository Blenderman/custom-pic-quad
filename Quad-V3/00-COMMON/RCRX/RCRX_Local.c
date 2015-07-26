#include "RCRX/RCRX_Local.h"
//==============================================================================
uint                _RCRX_IL	= 0; // Receiver (UART) interrupt level;
                                     // Also doubles as initialization flag
float               _RCRX_msecRate;  // Conversion rate to go from Timer TS to
                                     // milliseconds; set in RCRXInit(...)
//==============================================================================
// The following 4 entries are specific to receiver used; set in RCRXInit(...)
//==============================================================================
byte                _RCRX_Start = 0x00; // Matches LemonRX
byte                _RCRX_Stop  = 0xFF; // Same for LemonRX and OrangeRX
float               _RCRX_Swing = 512;  // Each channel swing from mid-point
//==============================================================================
// Receiver-specific variables
//------------------------------------------------------------------------------
BOOL    volatile    _RCRX_IsConnected   = FALSE;  // Naturally :)
BOOL    volatile    _RCRX_IsNewFrame    = FALSE;  // Obviously :)
ulong   volatile    _RCRX_LastReadTS    = 0;      // TS of last read
//-----------------------------------------------------
byte 	volatile    _RCRX_RX_NewFrame[_RCRX_FrameSize];
//==============================================================================
// State management variables
//==============================================================================
// State machine
//------------------------------------------------------------------------------
uint    const       _RCRX_RX_StateControl[7][3]	=
        { // Data   Start   Stop
            { 0,      1,     0 },   // 0: Scan to first START
            { 0,      2,     0 },   // 1: Expecting second START
            { 3,      3,     4 },   // 2: Frame START identified
            { 3,      3,     4 },   // 3: Process frame data
            { 5,      5,     6 },   // 4: First STOP?
            { 3,      3,     4 },   // 5: Data byte after miscellaneous STOP
            { 0,      1,     0 }    // 6: Frame terminated - valid frame is
                                    //    reported here!
        };
//------------------------------------------------------------------------------
// Receiver Buffer and control variables
//------------------------------------------------------------------------------
uint	volatile	_RCRX_RX_CurrentState		= 0;
// Length of the current data in receive buffer
uint	volatile	_RCRX_RX_FrameIdx	= 0;	// Empty...
// RCRX receive buffer
byte 	volatile    _RCRX_RX_FrameBuf[_RCRX_FrameSize];
//==============================================================================


