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
    TMRInit(2);        // Initialize Timer interface with Priority=2
    BLIInit();        // Initialize Signal interface
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
    I2CInit(5, 2);    // First param: IL = 5 (interrupt request priority
                    // Second param: I2C speed
                    // 0 - lowest (123 kHz at Fcy = 64MHz)
                    // 1 - 200 kHz - MPU-6050 stable
                    // 2 - 400 kHz
                    // 3 - 1 MHz
    //-------------------------------------------------------------------
    BLIAsyncStart(50,50);
    TMRDelay(3000);
    BLIAsyncStop();
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
    if ( MPLInit (0) )
        BLIDeadStop ("EB", 2);
    //*******************************************************************
    BLISignalOFF();
    //==================================================================
    ulong Alarm;
    ulong StartTS;
    struct
        {
        byte      OSR;
        byte      Avg;
        MPLData   MPLSample;
        } Msg;
    //==================================================================
    for (Msg.OSR = 0; Msg.OSR <= 7; Msg.OSR++)
        {
        MPLReset(Msg.OSR);
        Msg.Avg = 1 << Msg.OSR;
        MPLSetGround();
        Alarm = TMRSetAlarm(5000);
        StartTS = TMRGetTS();
        do
            {
            MPLReadSample(&Msg.MPLSample);
            //----------------------------
            Msg.MPLSample.TS -= StartTS;
            //----------------------------
            SDLPostIfReady(    (byte*) &Msg, sizeof(Msg));
            BLISignalFlip();
            } while (FALSE == TMRCheckAlarm(Alarm));
        }
    //==================================================================
    BLIDeadStop ("Z", 1);
    //*******************************************************************
    return 0;
    }

