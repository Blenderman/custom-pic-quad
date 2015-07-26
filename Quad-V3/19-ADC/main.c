#include "System.h"
//---------------------------------
#include "Init\Init.h"
#include "Init\Switches.h"
#include "TMR\TMR.h"
#include "BLI\BLI.h"
#include "SDL\SDL.h"
//---------------------------------
#include "ADC\ADC.h"    

int main(void)
    {

    //*******************************************************************
    Init();
    TMRInit(2);     // Initialize Timer interface with Priority=2
    BLIInit();      // Initialize Signal interface
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
    ADCInit(3);
    //==================================================================
    ulong       Alarm;
    struct
        {
        uint        RawData;
        uint        CellCount;
        float       NomVoltage;
        BATData     BatStatus;
        } ADCData;
    //==================================================================
    // Main Loop
    //-----------------------------------------------------
    BLISignalOFF();
    while (TRUE)
        {
        Alarm = TMRSetAlarm(500);
        //-----------------------------------------------------
        ADCData.RawData     = ADCGetRawSample();
        ADCData.CellCount   = ADCGetCellCount();
        ADCData.NomVoltage  = ADCGetBatteryNomVoltage();
        ADCGetBatteryStatus(&ADCData.BatStatus);
        //-----------------------------------------------------
        BLISignalFlip();
        //-------------------------
        SDLPostIfReady((byte*)&ADCData, sizeof(ADCData));
        //-------------------------
        TMRWaitAlarm(Alarm);
        }

    //*******************************************************************
    return 0;
    }

