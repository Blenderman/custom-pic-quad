#include "System.h"
//---------------------------------
#include "Init\Init.h"
#include "Init\Switches.h"
#include "TMR\TMR.h"
#include "BLI\BLI.h"
#include "SDL\SDL.h"
//---------------------------------
#include "MPL\MPL.h"

int main(void)
	{

	//*******************************************************************
	Init();
	TMRInit(2);		// Initialize Timer interface with Priority=2
	BLIInit();		// Initialize Signal interface
	//*******************************************************************
	// Switch 1 controls the Serial Data Logger (SDL) communication speed
	//-------------------------------------------------------------------
	if (_SW1)
		// Switch 1 is ON - Configuring SDL for PIC-to-PIC
		// high-speed communication at 1 MBaud
		SDLInit(3, BAUD_1M);
	else
		// Switch 1 is OFF - Configuring SDL for ZigBEE
		// wireless communication at 115.2 KBaud
		SDLInit(3, BAUD_115200);
	//*******************************************************************
	I2CInit(5, 3);
	//-------------------------------------------------------------------
	// Initialize MPL3115 Altimeter
	//------------------------------------------------------------------
	// OSR = 3 => Average over 2^3=  8 samples, update rate about 38 Hz
	//			  Sample-to-sample dev +/- 3 m
	// OSR = 4 => Average over 2^4= 16 samples, update rate about 20 Hz
	// OSR = 5 => Average over 2^5= 32 samples, update rate about 10 Hz
	// OSR = 6 => Average over 2^6= 64 samples, update rate about  6 Hz
	// OSR = 7 => Average over 2^7=128 samples, update rate about  3 Hz
	//------------------------------------------------------------------
	byte	OSR	= 3;
	//------------------------------------------------------------------
	if ( MPLInit (OSR) )
		BLIDeadStop ("EB", 2);
	//*******************************************************************
	BLISignalOFF();
	//==================================================================
	uint RC = 0;
	byte Data;
	//-----------------------------------------------------
	RC = MPLGetSTAT(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetID(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetMODE(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetINTSrc(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetCTRL1(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetCTRL2(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetCTRL3(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetCTRL4(&Data);
	RC = 0;
	//-----------------------------------------------------
	RC = MPLGetCTRL5(&Data);
	RC = 0;
	//-----------------------------------------------------

	//==================================================================
	MPLData		MPLSample;
	//-----------------------------------------------
//	while (TRUE)
//		{
//		MPLReadSample(&MPLSample);
//		SDLPostIfReady(	(byte*) &MPLSample, sizeof(MPLSample));
//		BLISignalFlip();
//		}


	//==================================================================
	// Start MPL3115 Altimeter in Asynchronous mode
	//--------------------------------------------------------------
	MPLAsyncStart();	// Start as soon as possible for warm-up
	//==================================================================
	// Calibrate ground level for MPL3115 Altimeter
	//--------------------------------------------------------------
	if (MPLSetGround() != MPL_OK) 
		// Altimeter calibration failed
		BLIDeadStop("CB", 2);	// Failure...
	//==================================================================
	// Altitude from Barometric sensor
	//-----------------------------------------------
	while (TRUE)
		{
		if (MPL_OK != MPLAsyncReadWhenReady(&MPLSample))
			BLIDeadStop("SOS", 3);
		//-----------------------------------------------------
		SDLPostIfReady(	(byte*) &MPLSample, sizeof(MPLSample));
		//-----------------------------------------------------
		BLISignalFlip();
		}
	//*******************************************************************
	return 0;
	}

