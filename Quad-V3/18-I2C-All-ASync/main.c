#include "System.h"
//---------------------------------
#include "Init\Init.h"
#include "Init\Switches.h"
#include "TMR\TMR.h"
#include "BLI\BLI.h"
#include "SDL\SDL.h"
//---------------------------------
#include "MPU\MPU.h"    // MPU-6050 - gyroscope/accelerometer
#include "MPL\MPL.h"    // MPL-3115A2 - barometric altimeter

int main(void)
    {

    //*******************************************************************
    Init();         // Initialize microprocessor
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
    //*******************************************************************
    // Initialize I2C Library
    //-------------------------------------------------------------------
    I2CInit(5, 2);  // First param: IL = 5 (interrupt request priority
                    // Second param: I2C speed
                    // 0 - lowest (123 kHz at Fcy = 64MHz)
                    // 1 - 200 kHz
                    // 2 - 400 kHz
                    // 3 - 1 MHz
    //*******************************************************************
    uint    RC      = 0;
    ulong   Alarm   = 0;
    //==================================================================
    // Initialize MPUs
    //------------------------------------------------------------------
    RC = MPUInit(0, 3, MPU_GYRO_2000ds, MPU_ACC_2g);
                        // Initialize motion Sensor
                        // 1 kHz/(0+1) = 1000 Hz (1msec)
                        // DLPF = 3 => Bandwidth 44 Hz (delay: 4.9 msec)
    if (RC)    BLIDeadStop("EG", 2);
    //==================================================================
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
    byte    OSR    = 3;
    //------------------------------------------------------------------
    if ( MPL_OK != MPLInit (OSR) )
        BLIDeadStop ("EB", 2);
    //==================================================================
    // Initialize Asynchronous mode
    //-----------------------------------------------------
    if ( (RC = MPUAsyncStart(1)) )
        BLIDeadStop("S1", 2);
    //------------------------------
    if ( (RC = MPUAsyncStart(2)) )
        BLIDeadStop("S2", 2);
    //-----------------------------------------------------
    if ( (RC = MPLAsyncStart()) )
        BLIDeadStop("S3", 2);
    //==================================================================
    // Provide a few second delay prior to calibrating Gyros to make
    // sure that the board is stable after the "turn-on" shake
    //------------------------------------------------------------------
    BLIAsyncStart(50,50);
    TMRDelay(5000);
    BLIAsyncStop();
    //==================================================================
    // Calibrate Gyros
    //------------------------------------------------------------------
    BLIAsyncStart(100,100);
    //------------------------------
    if ( (RC = MPUCalibrateGyro(1)) )
        BLIDeadStop("C1", 2);
    //------------------------------
    if ( (RC = MPUCalibrateGyro(2)) )
        BLIDeadStop("C2", 2);
    //------------------------------
    BLIAsyncStop();
    //==================================================================
    
    //==================================================================
    struct
        {
        MPUData        MPUSample1;
        MPUData        MPUSample2;
        MPLData        MPLSample;
        } SensorData;
    //==================================================================
    // Main Loop
    //------------------------------------------------------------------
    BLISignalOFF();
    while (TRUE)
        {
        Alarm = TMRSetAlarm(1000);
        //-----------------------------------------------------
        if ( (RC = MPUAsyncReadWhenReady(1, &SensorData.MPUSample1)) )
            BLIDeadStop("SOS", 3);
        //--------------------------
        if ( (RC = MPUAsyncReadWhenReady(2, &SensorData.MPUSample2)) )
            BLIDeadStop("SOS", 3);
        if (MPL_OK != MPLAsyncReadWhenReady(&SensorData.MPLSample))
            BLIDeadStop("SOS", 3);
        //-----------------------------------------------------
        BLISignalFlip();
        //-------------------------
        SDLPostIfReady((byte*)&SensorData, sizeof(SensorData));
        //-------------------------
        TMRWaitAlarm(Alarm);
        }

    //*******************************************************************
    return 0;
    }

