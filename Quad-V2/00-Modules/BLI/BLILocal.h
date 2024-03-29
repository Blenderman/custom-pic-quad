#include "BLI\BLI.h"
#include "TMR\TMRLocal.h"

//=====================================================
#ifndef __BLILocal_H
#define __BLILocal_H
//=====================================================
#define _BLIAsynchMax 128
//=====================================================
extern uint				_BLIInit;	// BLI initializtion flag
extern byte				_BLIIL;		// BLI default interrupt level
//-----------------------------------------------------
// BLI Asynch flag
// "0" - no asynchronous request outstanding
// _BLIAsync > 0 represents the size of the array of On-Off times
extern volatile uint	_BLIAsync;	
//-----------------------------------------------------
// Current position in the "timing" array
extern volatile uint	_BLIAsyncPos;
// On-Off "timing" array
extern volatile uint	_BLIAsyncTimes[_BLIAsynchMax];
//-----------------------------------------------------
// Callback function for asynchronous calls
//-----------------------------------------------------
void _BLICallBack (void);
//=====================================================
#endif


