#include "System.h"
//---------------------------------
#include "Init/Init.h"
#include "Init/Switches.h"
#include "TMR/TMR.h"
#include "BLI/BLI.h"
#include "SDL/SDL.h"
//---------------------------------
#include "RCRX/RCRX.h"


int main(void)
    {

    //*******************************************************************
    Init();         // Initialize microprocessor
    TMRInit(2);     // Initialize Timer interface with Priority=2
    BLIInit();      // Initialize Signal interface
    //==================================================================
    // Provide a 5 second delay prior to initialization 
    // to avoid false-start during programming by PIC Kit 3
    //------------------------------------------------------------------
    BLIAsyncStart(50,200);
    TMRDelay(5000);
    BLIAsyncStop();
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
    if ( RCRX_OK != RCRXInit(3, LemonRX) )
        BLIDeadStop("SOS", 3);
    //===================================================================
    BLIAsyncStart(30,300);
    RCRXArm();
    BLIAsyncStop();
    //===================================================================
    RCRXData    RCSample;
    BLISignalOFF();
    while (TRUE)
        {
        if (RCRX_OK == RCRXRead(&RCSample))
            {
            SDLPostIfReady((byte*)&RCSample, sizeof(RCSample));
            BLISignalFlip();
            }
        }
    //*******************************************************************
    return 0;
    }

