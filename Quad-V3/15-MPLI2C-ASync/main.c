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
    // OSR = 0 => No averaging ( 2^0= 1),   update rate about 166.6 Hz
    // OSR = 1 => Average 2^1=   2 samples, update rate about 111.1 Hz
    // OSR = 2 => Average 2^2=   4 samples, update rate about  67.8 Hz
    // OSR = 3 => Average 2^3=   8 samples, update rate about  37.7 Hz
    // OSR = 4 => Average 2^4=  16 samples, update rate about  20.1 Hz
    // OSR = 5 => Average 2^5=  32 samples, update rate about  10.4 Hz
    // OSR = 6 => Average 2^6=  64 samples, update rate about   5.3 Hz
    // OSR = 7 => Average 2^7= 128 samples, update rate about   2.7 Hz
    //------------------------------------------------------------------
	byte	OSR	= 3;
	//------------------------------------------------------------------
	if ( MPL_OK != MPLInit (OSR) )
		BLIDeadStop ("EB", 2);
	//*******************************************************************
	BLISignalOFF();
	//==================================================================
	MPLData		MPLSample;
	//==================================================================

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

