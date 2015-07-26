#include "System.h"
//---------------------------------
#include "Init/Init.h"
#include "Init/Switches.h"
#include "TMR/TMR.h"
#include "BLI/BLI.h"
#include "SDL/SDL.h"
#include "RCRX/RCRX.h"
//---------------------------------
#include "MCM/MCM.h"


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
    if (RCRX_OK != RCRXInit(4, LemonRX))
        BLIDeadStop("RCRX", 4);
    //*******************************************************************
    BLIAsyncStart(50,50);
    if (RCRX_OK != RCRXArm())
        BLIDeadStop("ARM", 3);
    BLIAsyncStop();
    //-------------------------------------------------------------------
    MCMInitF(MCM_Cross, 400, 0);
    //*******************************************************************
    RCRXData    RCSample;
    MCMData     MCInput;
    BLISignalOFF();
    //-------------------------------------------------------------------
    while (TRUE)
        {
        if (RCRX_OK != RCRXReadWhenReady(&RCSample))
            continue;
        //--------------------------------------
        MCInput.Throttle = RCSample.Throttle;
        MCInput.AdjRoll  = RCSample.Roll;
        MCInput.AdjPitch = RCSample.Pitch;
        MCInput.AdjYaw   = RCSample.Yaw;
        //--------------------------------------
        MCMSet(&MCInput);
        //--------------------------------------
        BLISignalFlip();
        } 
    //*******************************************************************
    return 0;
    }

