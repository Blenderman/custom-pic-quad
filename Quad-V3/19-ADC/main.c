#include "System.h"
//---------------------------------
#include "Init/Init.h"
#include "Init/Switches.h"
#include "TMR/TMR.h"
#include "BLI/BLI.h"
#include "SDL/SDL.h"
//---------------------------------
#include "MPU/MPU.h"	// MPU-6050 - gyroscope/accelerometer
#include "MPL/MPL.h"	// MPL-3115A2 - barometric altimeter

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
	//==================================================================
	BLIAsyncStart(50,50);
	TMRDelay(3000);
	BLIAsyncStop();
	//==================================================================
	// Initialize ADC
	//------------------------------------------------------------------
	//=====================================================
	// Main Loop
	//-----------------------------------------------------
	BLISignalOFF();
	while (TRUE)
		{
//		Alarm = TMRSetAlarm(1000);
//		//-----------------------------------------------------
//		if ( (RC = MPUAsyncReadWhenReady(1, &SensorData.MPUSample1)) )
//			BLIDeadStop("SOS", 3);
//		//--------------------------
//		if ( (RC = MPUAsyncReadWhenReady(2, &SensorData.MPUSample2)) )
//			BLIDeadStop("SOS", 3);
//		if (MPL_OK != MPLAsyncReadWhenReady(&SensorData.MPLSample))
//			BLIDeadStop("SOS", 3);
//		//-----------------------------------------------------
		BLISignalFlip();
//		//-------------------------
//		SDLPostIfReady((byte*)&SensorData, sizeof(SensorData));
//		//-------------------------
//		TMRWaitAlarm(Alarm);
		}

	//*******************************************************************
	return 0;
	}

