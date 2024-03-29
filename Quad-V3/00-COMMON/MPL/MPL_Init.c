#include "MPL\MPL_Local.h"
#include "I2C\I2C_Local.h"

#include <math.h>

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

//==================================================================
// <editor-fold defaultstate="collapsed" desc="uint MPLInit(byte OSR)">
//------------------------------------------------------------------
uint    MPLInit(byte OSR)
    {
    //************************************************
    // MPL module depends on I2C for communication with the
    // sensor, so we need to make sure that I2C is initialized...
    //************************************************
    byte    IL = I2CGetIL ();
    if (0 == IL)
        return I2CRC_NRDY;    // I2C Module is not initialized
    //---------------------------------------------------------
    if (_MPL_Init)
        return    MPL_OK;        // Avoid repeated initialization...
    //---------------------------------------------------------
    _MPL_Init  = 1;
    _MPL_IL    = IL;
    //************************************************

    //************************************************
    // MPL group of functions depend on timer providing
    // timestamps for interval measurement, thus we
    // make sure that timer is initialized.
    //************************************************
    TMRInitDefault ();

    //---------------------------------------------------------
    // Configure Interrupts
    //---------------------------------------------------------
    MPL_IE        = 0;         // Disable INTx interrupt
    MPL_IF        = 0;         // Clear INTx interrupt flag
    MPL_EP        = 0;         // Interrupt on POSITIVE edge
    MPL_IP        = _MPL_IL;   // Set INTx interrupt priority
    //---------------------------------------------------------
    // Map MCU pins used by INTx module through
    // REMAPPABLE PIN SELECT feature.
    //---------------------------------------------------------
    _MPLInitPinMap ();
    //---------------------------------------------------------
    // INTx interrupt enabled in MPLAsyncStart() routine
    //---------------------------------------------------------

    //---------------------------------------------------------
    // Now we should initialize the sensor...
    //---------------------------------------------------------
    return MPLReset (OSR);
    }
// </editor-fold>
//==================================================================

//==================================================================
// <editor-fold defaultstate="collapsed" desc="uint MPLReset(byte OSR)">
//------------------------------------------------------------------
uint    MPLReset(byte OSR)
    {
    if (!_MPL_Init)
        return MPL_NOTINIT;        // Not initialized...
    //*********************************************************
    uint        RC        = MPL_OK;
    //*********************************************************
    if (_MPL_Async)
        {
        // Asynchronous operation in progress...
        // Let's try to stop it for RESET
        if (MPL_OK != (RC = MPLAsyncStop()))
            return RC;    // Failure... Not likely :)
        }
    
    //************************************************
    // Validate OSR - acceptable values are from 0
    // for no oversampling to 7 for 2^7=128 over-
    // sampling ratio
    //************************************************
    if (OSR > 7) OSR = 7;
    //*********************************************************
    // Before we initiate RESET we need to calculate value of
    // MAX interval between consecutive data samples based on
    // the new OSR value
    //*********************************************************
    _MPL_MaxInt = (ulong)( (powf(2.0, (float)OSR)*0.004 + 0.003)
                / TMRGetTSRate());

    //*********************************************************
    byte        CtrlR1;
    byte        OSR_Ctrl = OSR << 3;
    //---------------------------------------------------------
    // First, let's put MPL into STANDBY mode
    //---------------------------------------------------------
    // Read current value
    RC = _MPLRead(CtrlR1Addr, &CtrlR1, 1);
    if (RC) return RC;    // Failure...
    // Clear SBYB bit
    CtrlR1 &= 0xFE;
    // Apply new configuration
    RC    = _MPLWrite(CtrlR1Addr, &CtrlR1, 1);
    if (RC) return RC;    // Failure...
    // Wait until STANDBY completes
    CtrlR1 = 0xFF;        // Dummy value
    while (CtrlR1 & 0x01)
        {
        TMRDelay(1);    // 1 msec delay...
        RC = _MPLRead(CtrlR1Addr, &CtrlR1, 1);
        if (RC) return RC;    // Failure...
        }
    //---------------------------------------------------------
    // Second, let's RESET MPL by initiating RESET
    // NOTE: RESET also disables FIFO
    //---------------------------------------------------------
    CtrlR1 = 0x04;        // Set RESET bit
    // Initiate RESET
    // NOTE: REST also resets I2C bus, so we expect NACK
    RC    = _MPLWrite(CtrlR1Addr, &CtrlR1, 1);
    if (RC != I2CRC_NACK) return RC;    // Failure...
    // Wait until RESET completes
    CtrlR1 = 0xFF;        // Dummy value
    while (0x00 != CtrlR1)
        {
        TMRDelay(1);    // 1 msec delay...
        RC = _MPLRead(CtrlR1Addr, &CtrlR1, 1);
        if (RC) return RC;    // Failure...
        }
    //---------------------------------------------------------
    // Now that MPL is in STANDBY after RESET, we may
    // configure other control registers
    //---------------------------------------------------------
    // Let's configure Event Flag Generation
    //---------------------------------------------------------
    byte    PT_DATA_CFG = 0x07; // DRDY = PDEFE = TDEFE = 1
    RC    = _MPLWrite(0x13, &PT_DATA_CFG, 1);
    if (RC) return RC;    // Failure...
    //---------------------------------------------------------
    // After RESET CtrlR2 = 0x00 - no need to change
    //---------------------------------------------------------
    // Interrupt Control Register
    byte    CtrlR3      = 0x22; // Int1, Int2 - active HIGH
                                // Push/Pull (vs. Open Drain)
    RC    = _MPLWrite(0x28, &CtrlR3, 1);
    if (RC) return RC;    // Failure...
    //---------------------------------------------------------
    // Interrupt Enable Register
    byte    CtrlR4      = 0x83; // Only DRDY + PCHG + TCHG
    RC    = _MPLWrite(0x29, &CtrlR4, 1);
    if (RC) return RC;    // Failure...
    //---------------------------------------------------------
    // Interrupt Routing Register
    byte    CtrlR5      = 0x83; // DRDY + PCHG + TCHG on Int1 pin
    RC    = _MPLWrite(0x2A, &CtrlR5, 1);
    if (RC) return RC;    // Failure...
    //---------------------------------------------------------
    // Finally we may configure sensor for Altimeter mode with
    // specified OSR and set OST to start the cycle
    //---------------------------------------------------------
    CtrlR1 = 0x80 | OSR_Ctrl | CtrlR1SetOST;    // Alt + OSR + OST
    // Write new configuration value
    RC = _MPLWrite(CtrlR1Addr, &CtrlR1, 1);
    if (RC) return RC;    // Failure...
    //*********************************************************

    //*********************************************************
    // To conclude RESET we need to implement single READ
    // operation
    //*********************************************************
    MPLData    Sample;
    return MPLReadSample(&Sample);
    }
// </editor-fold>
//==================================================================

//==================================================================
// <editor-fold defaultstate="collapsed" desc="uint MPLSetGround()">
//------------------------------------------------------------------
uint    MPLSetGround()
    {
    if (0 == _MPL_Init)
        return MPL_NOTINIT;        // Not initialized...
    //=========================================================
    // Local Variables
    //---------------------------------------------------------
    MPLData     SampleData;
    float       Ground      = 0.0;
    //---------------------------------------------------------
    uint        RC          = MPL_OK;    // Pre-set to Success
    //=========================================================

    //=========================================================
    // Reset  Ground offset...
    //---------------------------------------------------------
    // Clear accumulated pipeline
    if (_MPL_Async)
        {
        // Asynchronous operation in progress...
        if ( MPL_OK != (RC = MPLAsyncReadWhenReady(&SampleData)) )
            return RC;                // Error...
        }
    // Reset Base offset
    _MPL_BaseAlt    = 0.0;
    //---------------------------------------------------------
    // To collect average for Ground altitude we would like to
    // sample MPL for 1 seconds
    //---------------------------------------------------------
    ulong   Alarm = TMRSetAlarm(1000);  // Set Alarm time 1 sec
                                        // into the future
    long    SampleCount    = 0;         // Number of samples collected
    //---------------------------------------------------------
    do
        {
        if (_MPL_Async)
            // Asynchronous operation in progress...
            RC = MPLAsyncReadWhenReady(&SampleData);
        else
            // Use Synchronous access
            RC = MPLReadSample(&SampleData);
        //-----------------------------------------
        // Check for error
        //-----------------------------------------
        if ( MPL_OK != RC )
            return RC;                // Error...
        //--------------------------------------
        // Sample obtained successfully
        //--------------------------------------
        Ground += SampleData.Alt;    // Accumulate Alt value
        SampleCount++;                // Increase count
        }
    while ( FALSE == TMRCheckAlarm(Alarm) );
    //---------------------------------------------------------
    // Let's set the Ground level as the average of SampleCount
    // samples with rounding to sensor precision + 1
    //---------------------------------------------------------
    _MPL_BaseAlt    =  floorf((Ground / ((float)SampleCount)) * 32.0 + 0.5)/32.0;
    //*********************************************************
    return MPL_OK;
    }
// </editor-fold>
//==================================================================

//==================================================================
// <editor-fold defaultstate="collapsed" desc="uint MPLAdjustGround(...)">
//------------------------------------------------------------------
uint    MPLAdjustGround(float Altitude, float Target)
    {
    if (0 == _MPL_Init)
        return MPL_NOTINIT;        // Not initialized...
    //=========================================================
    // Gradually adjust Ground offset...
    //---------------------------------------------------------
    // "Weight" should be some prime number different from
    // parameter "smoothing weight" in Altimeter module
    //---------------------------------------------------------
    // 131 - expect slow changes
    // 179 - even slower (for OSR = 3)
    // 257 - even slower (for OSR = 3)
    _MPL_BaseAlt = _MPL_BaseAlt + (Altitude - Target) / 257.0;
    //---------------------------------------------------------
    // Let's round the Ground level offset to sensor precision + 1
    //---------------------------------------------------------
    _MPL_BaseAlt    =  floorf(_MPL_BaseAlt * 32.0 + 0.5)/32.0;
    //*********************************************************
    return MPL_OK;
    }
// </editor-fold>
//==================================================================


