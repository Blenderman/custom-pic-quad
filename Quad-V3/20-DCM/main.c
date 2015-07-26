#include "System.h"
//---------------------------------
#include "Init\Init.h"
#include "Init\Switches.h"
#include "TMR\TMR.h"
#include "BLI\BLI.h"
#include "SDL\SDL.h"
//---------------------------------
#include "MPU\MPU.h"        // MPU-6050 - gyroscope/accelerometer
#include "HMCSPI\HMCSPI.h"  // HMC5983 - magnetometer
//---------------------------------
#include "DCM\DCM.h"        // DCM library

int main(void)
    {

    //*******************************************************************
    Init();         // Initialize microprocessor
    TMRInit(2);     // Initialize Timer interface with Priority=2
    BLIInit();      // Initialize Signal interface
    //==================================================================
    // Provide a 5 second delay prior to initialization of I2C interface
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
    //==================================================================
    // Initialize I2C Library
    //-------------------------------------------------------------------
    I2CInit(5, 2);  // First param: IL = 5 (interrupt request priority
                    // Second param: I2C speed
                    // 0 - lowest (123 kHz at Fcy = 64MHz)
                    // 1 - 200 kHz
                    // 2 - 400 kHz
                    // 3 - 1 MHz
    //*******************************************************************

    //==================================================================
    // <editor-fold defaultstate="collapsed" desc="Initializing Sensors">
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="HMC5983 Magnetometer initialization">
    //-------------------------------------------------------------------
    byte IL        = 5;    // Interrupt level
    //------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Output Data Rate (ODR">
    //------------------------------------------------------
    // Typical Data Output Rate (Hz) for various ODR values
    //------------------------------------------------------
    //        ODR =    0:          0.75
    //        ODR =    2:          1.5
    //        ODR =    2:          3
    //        ODR =    3:          7.5
    //        ODR =    4:         15        (Default)
    //        ODR =    5:         30
    //        ODR =    6:         75
    //        ODR =    7:        220        (fastest)
    // </editor-fold>
    byte ODR    = 7;    // Rate (fastest):    220 Hz
    // <editor-fold defaultstate="collapsed" desc="Low-pass filtering (DLPF)">
    //------------------------------------------------------
    // Low-pass filtering achieved through sample averaging.
    // Averaging does not affect effective ODR, so I assume
    // that with averaging enabled each successive reported
    // sample is an average of the new measurement with "n"
    // previous measurements - something like a FIR filter.
    //------------------------------------------------------
    // DLPF = 0 => 1-average    
    // DLPF = 1 => 2-average
    // DLPF = 2 => 4-average
    // DLPF = 3 => 8-average
    //------------------------------------------------------
    // </editor-fold>
    byte DLPF   = 2;    // If we use less than DLPF=2 results
                        // show some single-spike artefacts
    // <editor-fold defaultstate="collapsed" desc="Sensor Field Range (Gain)">
    //------------------------------------------------------
    // Recommended sensor field range (Ga) for various Gains
    //------------------------------------------------------
    //        Gain =    0:        0.9
    //        Gain =    1:        1.3
    //        Gain =    2:        1.9
    //        Gain =    3:        2.5
    //        Gain =    4:        4.0
    //        Gain =    5:        4.7
    //        Gain =    6:        5.6
    //        Gain =    7:        8.1
    //------------------------------------------------------
    // The magnitude of Earth magnetic field varies over the
    // surface of the Earth in the range 0.3 to 0.6 Gauss.
    //------------------------------------------------------
    // </editor-fold>
    byte Gain   = 1;    // +/- 1.3 Ga
    //------------------------------------------------------
    HMC_Init(IL, ODR, Gain, DLPF);
    //------------------------------------------------------
    if ( HMC_OK != HMC_AsyncStart() ) 
        BLIDeadStop("SH", 2);
    //-------------------------------------------------------------------
    // </editor-fold>
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Initialize MPU6050 IMUs">
    //------------------------------------------------------------------
    // Initialize motion Sensor
    // 1 kHz/(0+1) = 1000 Hz (1msec)
    // DLPF = 3 => Bandwidth 44 Hz (delay: 4.9 msec)
    if ( MPU_OK != MPUInit(0, 1, MPU_GYRO_2000ds, MPU_ACC_2g) )
        BLIDeadStop("SM", 2);
    //-------------------------------------------------------------------
    // </editor-fold>
    //------------------------------------------------------------------- 

    //------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Initialize Asynchronous mode for I2C sensors">
    //-----------------------------------------------------
    if ( MPUAsyncStart(1) ) 
        BLIDeadStop("SM1", 2);
    //------------------------------
    if ( MPUAsyncStart(2) ) 
        BLIDeadStop("SM2", 2);
    //------------------------------------------------------------------
    // </editor-fold>
    //------------------------------------------------------------------

    //==================================================================
    // Provide a few second delay prior to calibrating Gyros to make
    // sure that the board is stable after the "turn-on" shake
    //------------------------------------------------------------------
    BLIAsyncStart(50, 50);
    TMRDelay(1000);
    BLIAsyncStop();
    //==================================================================
    
    // <editor-fold defaultstate="collapsed" desc="Calibrate Gyros">
    //------------------------------------------------------------------
    BLIAsyncStart(100, 100);
    //------------------------------
    if ( MPUCalibrateGyro(1) )
        BLIDeadStop("C1", 2);
    //------------------------------
    if ( MPUCalibrateGyro(2) )
        BLIDeadStop("C2", 2);
    //------------------------------
    BLIAsyncStop(); // </editor-fold>
    //==================================================================
    // </editor-fold>
    //==================================================================
    struct
        {
        MPUData     MPUSample;
        HMCData     HMCSample;
        DCMData     DCMOut;
        } Data;
    //==================================================================
    // Initialize DCM
    //------------------------------------------------------------------
    TMRDelay(200);  // Accumulate average of multiple measurements
    //------------------------------------------------------------------
    if ( HMC_OK != HMC_AsyncReadWhenReady(&Data.HMCSample) )
        BLIDeadStop("SHMC", 4);
    //-----------------------------------------------------
    if ( MPU_OK != MPUAsyncReadCombined(&Data.MPUSample) ) 
        BLIDeadStop("SMPU", 4);
    //-----------------------------------------------------
    // Options (in order): UseAcc, UseMag, UseMagYawOnly:
    DCMReset(TRUE, TRUE, TRUE,
             &Data.MPUSample.A, &Data.HMCSample.M);
    //==================================================================
    // Main Loop
    //------------------------------------------------------------------
    BLISignalOFF();
    uint    Step = 0;
    while (TRUE)
        {
        //-----------------------------------------------------
        HMC_AsyncReadIfReady(&Data.HMCSample);
        //-----------------------------------------------------
        // In general, a blocking call, but as MPU-6050 is
        // running at 1 kHz, by the time we get here a
        // new sample should be ready and returned immediately.
        //-----------------------------------------------------
        if ( MPU_OK != MPUAsyncReadCombined(&Data.MPUSample) )
            BLIDeadStop("SOS", 3);
        //-----------------------------------------------------
        if (DCM_OK != DCMUpdate( Data.MPUSample.TS,	// Gyro Timestamp
                                &Data.MPUSample.G,
                                 Data.MPUSample.TS,	// Acc Timestamp
                                &Data.MPUSample.A,
                                 Data.HMCSample.TS, // Mag Timestamp
                                &Data.HMCSample.M,
                                &Data.DCMOut))
            BLIDeadStop("SSS", 3);
        Step++;
        //-----------------------------------------------------
        if (Step >= 100)    // 214: roughly every 0.5 sec
            {
            Step = 0;
            //-------------------------
            BLISignalFlip();
            //-------------------------
            SDLPostIfReady( (byte*)&Data, sizeof(Data));
            }
        //-------------------------
        }

    //*******************************************************************
    return 0;
    }

