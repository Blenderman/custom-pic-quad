#include "System.h"
//---------------------------------
#include "Init\Init.h"
#include "TMR\TMR.h"
#include "BLI\BLI.h"
#include "RC\RC.h"
#include "UART\UART_TX.h"

int main(void)
	{
	//*******************************************************************
	Init();
	//-----------------------------------------------------
	// Reset respective pins
	//-----------------------------------------------------
	_TRISB7		= 0;
	_LATB7		= 0;
	//--------------
	_TRISB8		= 0;
	_LATB8		= 0;
	//--------------
	_TRISB9		= 0;
	_LATB9		= 0;
	//--------------
	_TRISC6		= 0;
	_LATC6		= 0;

	//*******************************************************************
	TMRInit(2);			// Initialize Timer interface with Priority=2
	BLIInit();			// Initialize Signal interface
	//--------------------------
	BLISignalON();
	//--------------------------
	RCInit(4);			// Initialize Receiver interface with Priority=4
	//--------------------------
	UARTInitTX(6, 48);	// Initialize UART1 for TX on IPL=6 at 115200 bps
	// This initialization routine accepts BaudRate in multiples
	// of 2400 bps; Thus:
	// BaudRate =   1	=>   2400 bps
	// BaudRate =   2	=>   4800 bps
	// ...
	// BaudRate =  48	=> 115200 bps
	//------------------------------------------------------------
	// High speed
	//------------------------------------------------------------
	// BaudRate =  100	=>   250,000 bps
	// BaudRate =  200	=>   500,000 bps
	// BaudRate =  250	=>   625,000 bps
	// BaudRate =  350	=>   833,333 bps
	// BaudRate =  500	=> 1,250,000 bps
	// BaudRate = 1000	=> 2,500,000 bps
	//*******************************************************************
	TMRDelay(2000); 	// Wait for extra 2 sec - to let ESC arm...
						
	BLISignalOFF();
	TMRDelay(2000); 	// Wait for extra 2 sec - to let ESC arm...
						// (finish the song :) )
	//==================================================================
	RCData		RC;
	//-------------------------------------------------
	BLIAsyncMorse("R", 1);	// dot-doh-dot
	RCArm();
	BLIAsyncStop();
	//-------------------------------------------------
	BLISignalON();
	while(1)
		{
		RCReadWhenReady(&RC);
		//---------------------------------------------	
		UARTPostWhenReady((uchar*)&RC, sizeof(RC));
		//---------------------------------------------	
		BLISignalFlip();
		}
	return 1;
	}



