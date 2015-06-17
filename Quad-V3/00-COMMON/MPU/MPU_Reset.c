#include "MPU\MPU_Local.h"


//=============================================================
// <editor-fold defaultstate="collapsed" desc="MPUReset(...)">
uint    MPUReset(uint MPUx, byte RateDiv, byte DLPF,
                 MPU_FS_SEL GS, MPU_AFS_SEL AS)
    {
    if (0 == _MPU_Init)
        return MPU_NOTINIT;        // Not initialized...
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)
        return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    if (pCB->_MPU_Async)
        return MPU_ABSY;    // Asynchronous operation in progress...
    //*********************************************************
    uint        RC;    
    byte        RegValue;
    ulong       Alarm;
    //*********************************************************
    // <editor-fold defaultstate="collapsed" desc="RESETting sensor and then taking it from STANDBY">
    //---------------------------------------------------------
    // First, for consistency, let's RESET MPU - all registers
    // are set to 0x00, except the MPU6050_PWR_MGMT_1=0x40 (sleep)
    //---------------------------------------------------------
    if (MPU_OK != (RC = MPUSetByte(MPUx, MPU6050_PWR_MGMT_1, 0x80)))
        return RC;    // Failure...
    // RESET bit is set to 0 by HW at the end of RESET
    // Wait until RESET completes
    Alarm    = TMRSetAlarm(10);    // 10 msec should be enough for reset
    do 
        {
        TMRDelayTicks(1);    // Short Delay
        //--------------------------------
        if (MPU_OK != (RC = MPUGetByte(MPUx, MPU6050_PWR_MGMT_1, &RegValue)))
            return RC;    // Failure...
        //--------------------------------
        if (TMRCheckAlarm(Alarm))    
            return MPU_FAIL;    // Time expired without MPU completing RESET
        }
    while (0x40 != RegValue);
    //---------------------------------------------------------
    // Second, we have to take MPU out of "sleep" and configure
    // to use PLL with X axis (fastest!) gyroscope reference
    //---------------------------------------------------------
    if (MPU_OK != (RC = MPUSetByte(MPUx, MPU6050_PWR_MGMT_1, 0x01)))
        return RC;          // Failure...
    // Verify PWR_MGMT_1
    if (MPU_OK != (RC = MPUGetByte(MPUx, MPU6050_PWR_MGMT_1, &RegValue)))
        return RC;          // Failure...
    if (0x01 != RegValue) 
        return MPU_FAIL;    // Initialization failed
    //---------------------------------------------------------
    // Third, we have to take ALL sensors out of STANDBY
    // Should be by-default after RESET, but...
    //---------------------------------------------------------
    if (MPU_OK != (RC = MPUSetByte(MPUx, MPU6050_PWR_MGMT_2, 0x00)))
        return RC;          // Failure...
    // Verify PWR_MGMT_2
    if (MPU_OK != (RC = MPUGetByte(MPUx, MPU6050_PWR_MGMT_2, &RegValue)))
        return RC;          // Failure...
    if (0x00 != RegValue) 
        return MPU_FAIL;    // Initialization failed
    // </editor-fold>
    //=========================================================
    // <editor-fold defaultstate="collapsed" desc="Configuring DLPF">
    //---------------------------------------------------------
    // Digital low-pass filter: when DLPF = 0 - disabled,
    // when DLPF = 1...6 - enabled, DLPF = 7 - reserved
    // (DLPF occupies the lowest 3 bits of the register)
    if (DLPF > 6)    DLPF = 6;  // Highest allowed value
    if (DLPF < 1)    DLPF = 1;  // Lowest allowed value (to keep
                                // Gyro and Acc rates in sync)
    if (MPU_OK != (RC = MPUSetByte(MPUx, MPU6050_CONFIG, DLPF)))
        return RC;    // Failure...
    // Verify written value
    if (MPU_OK != (RC = MPUGetByte(MPUx, MPU6050_CONFIG, &RegValue)))
        return RC;    // Failure...
    if (DLPF != RegValue)
        return MPU_FAIL; // Initialization failed
    // </editor-fold>
    //=========================================================
    // <editor-fold defaultstate="collapsed" desc="Configuring Rate Divider">
    //---------------------------------------------------------
    // Sample Rate = Gyroscope Output Rate / (1 + RateDiv) where
    // Gyroscope Output Rate = 1kHz when the DLPF is enabled
    // ( 1 <= DLPF <= 6)
    if (MPU_OK != (RC = MPUSetByte(MPUx, MPU6050_SMPLRT_DIV, RateDiv)))
        return RC;    // Failure...
    // Verify written value
    if (MPU_OK != (RC = MPUGetByte(MPUx, MPU6050_SMPLRT_DIV, &RegValue)))
        return RC;    // Failure...
    if (RateDiv != RegValue)
        return MPU_FAIL; // Initialization failed
    // </editor-fold>
    //=========================================================
    // <editor-fold defaultstate="collapsed" desc="Gyro configuration">
    // XG_ST (self-test)        0
    // YG_ST (self-test)        0
    // ZG_ST (self-test)        0
    // FS_SEL[1:0]
    //                          00    -  250 deg/sec
    //                          01    -  500 deg/sec
    //                          10    - 1000 deg/sec
    //                          11    - 2000 deg/sec
    // Reserver [2:0]          000
    //---------------------------------------------------------
    // NOTE: Gyro sensitivity coefficient is set in such a way
    //       thar raw Gyro values are converted to Rad/sec
    //---------------------------------------------------------
    switch (GS)
        {
        case     MPU_GYRO_250ds:
            pCB->_MPU_Gyro_Sensitivity = 0.00013323124;
            RegValue = 0x00;
            break;
        case     MPU_GYRO_500ds:
            pCB->_MPU_Gyro_Sensitivity = 0.00026646248;
            RegValue = 0x01;
            break;
        case     MPU_GYRO_1000ds:
            pCB->_MPU_Gyro_Sensitivity = 0.00053211257;
            RegValue = 0x02;
            break;
        case     MPU_GYRO_2000ds:
        default:
            pCB->_MPU_Gyro_Sensitivity = 0.0010642251;
            RegValue = 0x03;
            break;
        }
    RegValue *= 8;    // Place sensitivity bits into proper position
    RC    = MPUSetByte(MPUx, MPU6050_GYRO_CONFIG, RegValue);
    if (RC) return RC;    // Failure...
    // </editor-fold>
    //=========================================================
    // <editor-fold defaultstate="collapsed" desc="ACC configuration">
    // XA_ST (self-test)        0
    // YA_ST (self-test)        0
    // ZA_ST (self-test)        0
    // AFS_SEL[1:0]
    //                          00    -  2g
    //                          01    -  4g
    //                          10    -  8g
    //                          11    - 16g
    // Reserver [2:0]          000
    switch (AS)
        {
        case     MPU_ACC_2g:
            pCB->_MPU_Accl_Sensitivity = 0.000061035156;
            RegValue = 0x00;
            break;
        case     MPU_ACC_4g:
            pCB->_MPU_Accl_Sensitivity = 0.00012207031;
            RegValue = 0x01;
            break;
        case     MPU_ACC_8g:
            pCB->_MPU_Accl_Sensitivity = 0.00024414062;
            RegValue = 0x02;
            break;
        case     MPU_ACC_16g:
        default:
            pCB->_MPU_Accl_Sensitivity = 0.00048828125;
            RegValue = 0x03;
            break;
        }
    RegValue *= 8;    // Place sensitivity bits into proper position
    RC    = MPUSetByte(MPUx, MPU6050_ACCEL_CONFIG, RegValue);
    if (RC) return RC;    // Failure...
    // </editor-fold>
    //=========================================================
    // <editor-fold defaultstate="collapsed" desc="Disable FIFO, I2C Master, and Sensor Path">
    //---------------------------------------------------------
    // Now that sensors are configured, we may proceed to
    // DISABLE FIFO and I2C Master, and RESET sensor's
    // Signal Path and respective registers
    //---------------------------------------------------------
    if (MPU_OK != (RC = MPUSetByte(MPUx, MPU6050_USER_CTRL, 0x01)))
        return RC;          // Failure...
    // Sensor's RESET bits are returned to 0 by HW at the end of
    // RESET - let's wait until RESET completes
    // Wait until RESET completes
    Alarm    = TMRSetAlarm(10);    // 10 msec should be enough for reset
    do
        {
        TMRDelayTicks(1);       // Short Delay
        //--------------------------------
        if (MPU_OK != (RC = MPUGetByte(MPUx, MPU6050_USER_CTRL, &RegValue)))
            return RC;          // Failure...
        //--------------------------------
        if (TMRCheckAlarm(Alarm))    
            return MPU_FAIL;    // Time expired without MPU completing RESET
        }
    while (0x00 != RegValue);
    // </editor-fold>
    //=========================================================
    // Now we may proceed with configuring interrupts
    //=========================================================
    // <editor-fold defaultstate="collapsed" desc="Configuring interrupts">
    // Interrupt control register
    //---------------------------------------------
    // INT Active High      (0)
    // INT push-pull        (0)
    // INT pulse 50 usec    (0)
    // Any READ clears INT  (1)
    //---------------------
    // FSYNC disabled [3:2] (0)
    //---------------------
    // I2C_BYPAS_EN         (0)
    // CLKOUT disabled       (0)
    //---------------------------------------------
    RC    = MPUSetByte(MPUx, MPU6050_INT_PIN_CFG, 0x10);
    if (RC) return RC;    // Failure...
    //=========================================================
    // Enable interrupts: Only DATA_RDY_EN
    RC    = MPUSetByte(MPUx, MPU6050_INT_ENABLE, 0x01);
    if (RC) return RC;    // Failure...
    //=========================================================
    //Clear READY status (if any) by reading STATUS register
    RC    = MPUGetByte(MPUx, MPU6050_INT_STATUS, &RegValue);
    if (RC) return RC;    // Failure...
    //*********************************************************
    // </editor-fold>
    //=========================================================

    return MPU_OK;
    }
// </editor-fold>
//=============================================================

//=============================================================
// <editor-fold defaultstate="collapsed" desc="MPUCalibrate(...)">
//------------------------------------------------------------------
// Forward declaration: Synchronous Gyro Calibration routine
//------------------------------------------------------------------
uint    _MPUCalibrateGyroSync( uint MPUx, Vector* pBase);
//------------------------------------------------------------------
// Forward declaration: Asynchronous Gyro Calibration routine
//------------------------------------------------------------------
uint    _MPUCalibrateGyroAsync(uint MPUx, Vector* pBase);
//=============================================================
uint    MPUCalibrateGyro(uint MPUx)
    {
    if (!_MPU_Init)
        return MPU_NOTINIT;        // Not initialized...
    //---------------------------------------------------------
    MPU_CB*     pCB = MPUpCB(MPUx);
    if (NULL == pCB)
        return MPU_NOTA;    // Should never happened!
    //=========================================================
    // Reset  Gyro offsets...
    //---------------------------------------------------------
    VectorSet(0.0, 0.0, 0.0,    &pCB->_MPU_Gyro_Base);
    //---------------------------------------------------------
    if (pCB->_MPU_Async)
        return _MPUCalibrateGyroAsync(MPUx, &pCB->_MPU_Gyro_Base);
    else
        return _MPUCalibrateGyroSync( MPUx, &pCB->_MPU_Gyro_Base);
    //-----------------------
    }
//=============================================================
// </editor-fold>
//=============================================================

//=============================================================
// <editor-fold defaultstate="collapsed" desc="MPUCalibrate(...) Helper functions">
//=============================================================
// <editor-fold defaultstate="collapsed" desc="_MPUCalibrateSync(...)">
uint    _MPUCalibrateGyroSync(uint MPUx, Vector* pBase)
    {
    //=========================================================
    // Local Variables
    //---------------------------------------------------------
    ulong Alarm    = TMRSetAlarm(2000); // Set Alarm time 2 sec
                                        // into the future.
    //---------------------------------------------------------
    MPUData     Sample;
    //------------------------
    Vector      SumSample;
    VectorSet(0.0, 0.0, 0.0,    &SumSample);
    //------------------------
    long            SampleCount    = 0;
    //------------------------
    uint            RC            = MPU_OK;    // Pre-set to Success
    //=========================================================

    //---------------------------------------------------------
    // To collect averages we would like to "watch" MPU for at
    // least 2 seconds; number of samples will be variable
    // depending on the value of RateDiv
    //---------------------------------------------------------
    do
        {
        if ( MPU_OK != (RC = MPUReadSample(MPUx, &Sample)) )
            return RC;  // Error...
        //------------------
        VectorAdd(&SumSample, &Sample.G, &SumSample);
        //------------------
        SampleCount++;
        }
    while ( !TMRCheckAlarm(Alarm) );
    //-----------------------------------------------------
    // Calculate Gyro offset based upon multiple samples
    //-----------------------------------------------------
    if (SampleCount > 0)
        {
        VectorScale(&SumSample, 1.0/(float)SampleCount, pBase);
        return MPU_OK;
        }
    //*********************************************************
    return MPU_FAIL;
    }
// </editor-fold>
//=============================================================
// <editor-fold defaultstate="collapsed" desc="_MPUCalibrateAsync(...)">
uint    _MPUCalibrateGyroAsync(uint MPUx, Vector* pBase)
    {
    //=========================================================
    // Local Variables
    //---------------------------------------------------------
    MPUData     Sample;
    //------------------------
    uint        RC        = MPU_OK;    // Pre-set to Success
    //=========================================================

    //=========================================================
    // Consume current sample to clean the buffer
    //---------------------------------------------------------
    if (MPU_OK != (RC = MPUAsyncReadWhenReady(MPUx, &Sample)))
        return RC;
    //---------------------------------------------------------
    // To collect averages we would like to "watch" MPU for at
    // least 2 seconds; number of samples will be variable
    // depending on the value of RateDiv
    //---------------------------------------------------------
    TMRDelay(2000);
    //=========================================================
    // Read MPU Sample - MPUAsyncReadWhenReady(...) already
    // implemented averaging over collected samples!
    //---------------------------------------------------------
    if (MPU_OK != (RC = MPUAsyncReadWhenReady(MPUx, &Sample)))
        return RC;    
    //=========================================================
    // Save current value of Gyro components as new Gyro Base
    // vector
    VectorCopy(&Sample.G, pBase);
    //*********************************************************
    return MPU_OK;
    }
// </editor-fold>
//=============================================================
// </editor-fold>
//=============================================================




