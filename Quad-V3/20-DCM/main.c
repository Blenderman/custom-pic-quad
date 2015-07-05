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
#include "MPL\MPL.h"        // MPL-3115A2 - barometric altimeter
#include "ADC\ADC.h"        // ADC - battery management
//---------------------------------
//#include "DML\DML.h"        // Data management library

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
    //==================================================================
    // Initialize ADC
    //------------------------------------------------------------------
    ADCInit(3);
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
    uint    RC      = 0;
    ulong   Alarm   = 0;

    //==================================================================
    // <editor-fold defaultstate="collapsed" desc="Initializing Sensors">
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="HMC5983 Magnetometer initialization">
    //-------------------------------------------------------------------
    byte IL        = 4;    // Interrupt level
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
    if ( HMC_OK != (RC = HMC_AsyncStart()) )
        BLIDeadStop("SOS", 3);
    //-------------------------------------------------------------------
    // </editor-fold>
    //-------------------------------------------------------------------

    //-------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Initialize MPU6050 IMUs">
    //------------------------------------------------------------------
    RC = MPUInit(0, 3, MPU_GYRO_2000ds, MPU_ACC_2g);
    // Initialize motion Sensor
    // 1 kHz/(0+1) = 1000 Hz (1msec)
    // DLPF = 3 => Bandwidth 44 Hz (delay: 4.9 msec)
    if (RC)    BLIDeadStop("EG", 2);
    //-------------------------------------------------------------------
    // </editor-fold>
    //------------------------------------------------------------------- 
    
    //------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Initialize MPL3115 Altimeter">
    //------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="OSR values description">
    // OSR = 0 => No averaging ( 2^0= 1),   update rate about 166.6 Hz
    // OSR = 1 => Average 2^1=   2 samples, update rate about 111.1 Hz
    // OSR = 2 => Average 2^2=   4 samples, update rate about  67.8 Hz
    // OSR = 3 => Average 2^3=   8 samples, update rate about  37.7 Hz
    // OSR = 4 => Average 2^4=  16 samples, update rate about  20.1 Hz
    // OSR = 5 => Average 2^5=  32 samples, update rate about  10.4 Hz
    // OSR = 6 => Average 2^6=  64 samples, update rate about   5.3 Hz
    // OSR = 7 => Average 2^7= 128 samples, update rate about   2.7 Hz
    // </editor-fold>
    byte    OSR    = 3;
    //------------------------------------------------------------------
    if ( MPL_OK != MPLInit (OSR) )
        BLIDeadStop ("EB", 2);
    //------------------------------------------------------------------
    // </editor-fold>
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Initialize Asynchronous mode for I2C sensors">
    //-----------------------------------------------------
    if ( (RC = MPUAsyncStart(1)) )
        BLIDeadStop("S1", 2);
    //------------------------------
    if ( (RC = MPUAsyncStart(2)) )
        BLIDeadStop("S2", 2);
    //-----------------------------------------------------
    if ( (RC = MPLAsyncStart()) )
        BLIDeadStop("S3", 2);
    //------------------------------------------------------------------
    // </editor-fold>
    //------------------------------------------------------------------

    //==================================================================
    // Provide a few second delay prior to calibrating Gyros to make
    // sure that the board is stable after the "turn-on" shake
    //------------------------------------------------------------------
    BLIAsyncStart(50, 50);
    TMRDelay(5000);
    BLIAsyncStop();
    //==================================================================
    
    // <editor-fold defaultstate="collapsed" desc="Calibrate Gyros">
    //------------------------------------------------------------------
    BLIAsyncStart(100, 100);
    //------------------------------
    if ( (RC = MPUCalibrateGyro(1)) )
        BLIDeadStop("C1", 2);
    //------------------------------
    if ( (RC = MPUCalibrateGyro(2)) )
        BLIDeadStop("C2", 2);
    //------------------------------
    BLIAsyncStop(); // </editor-fold>
    //==================================================================
    // </editor-fold>
    //==================================================================
    
    //==================================================================
    struct
        {
        MPUData     MPUSample;
        HMCData     HMCSample;
        MPLData     MPLSample;
        BATData     BatStatus;
        } SensorData;
    //==================================================================
    // Main Loop
    //------------------------------------------------------------------
    BLISignalOFF();
    while (TRUE)
        {
        Alarm = TMRSetAlarm(1000);
        //-----------------------------------------------------
        if ( (RC = MPUAsyncReadCombined(&SensorData.MPUSample)) )
            BLIDeadStop("SOS", 3);
        //-----------------------------------------------------
        if ( HMC_OK != (RC = HMC_AsyncReadIfReady(&SensorData.HMCSample)) )
            BLIDeadStop("SOS", 3);
        //-----------------------------------------------------
        if (MPL_OK != MPLAsyncReadIfReady(&SensorData.MPLSample))
            BLIDeadStop("SOS", 3);
        //-----------------------------------------------------
        ADCGetBatteryStatus(&SensorData.BatStatus);
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

