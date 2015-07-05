#include "MPU\MPU_Local.h"


// Forward declaration...
uint    _MPUAsyncRead(MPU_CB* pCB, MPUData* pSample);

//*************************************************************
uint        MPUAsyncStart(uint MPUx)
    {
    if (!_MPU_Init)
        return MPU_NOTINIT; // Not initialized...
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)
        return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    if (pCB->_MPU_Async)
        return MPU_OK;          // Already started...
    //------------------------------------------------------------------
    pCB->_MPU_Async    = 1;     // Set indicator that MPU6050 
                                // is in Asynchronous mode
    pCB->_MPU_Ready    = 0;     // Discard async sample, if any
    //=========================================================
    MPUSetIF(MPUx, 0);          // Clear the interrupt flag
    MPUSetIE(MPUx, 1);          // Enable interrupt processing
                                // When MPL6050 has sample, the
                                // interrupt will be triggered
    //=========================================================
    return MPU_OK;
    }
//*************************************************************


//*************************************************************
uint    MPUAsyncStop(uint MPUx)
    {
    if (!_MPU_Init)
        return MPU_NOTINIT; // Not initialized...
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)
        return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    if (0 == pCB->_MPU_Async)
        return MPU_OK;     // Async is not active...
    //=========================================================
    pCB->_MPU_Async    = 0;     // Clear MPU-6050 Asynchronous
                                // mode indicator
    pCB->_MPU_Ready    = 0;     // Discard async sample, if any
    //---------------------------------------------------------
    MPUSetIE(MPUx, 0);          // Disable interrupt processing
    MPUSetIF(MPUx, 0);          // Clear the interrupt flag
    //---------------------------------------------------------
    return MPU_OK;            
    }
//*************************************************************


//*************************************************************
uint    MPUAsyncReadIfReady(uint MPUx, MPUData* pSample)
    {
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)
        return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    if (0 == pCB->_MPU_Async)
        return MPU_NACT;    // Asynchronous read not active...
    //--------------------------------------------------
    if (0 == pCB->_MPU_Ready)
        return MPU_NRDY;
    //--------------------------------------------------
    return _MPUAsyncRead(pCB, pSample);
    }
//*************************************************************

//*************************************************************
uint    MPUAsyncReadWhenReady(uint MPUx, MPUData* pSample)
    {
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)    return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    if (0 == pCB->_MPU_Async)
        return MPU_NACT;            // Asynchronous read not active...
    //--------------------------------------------------
    while (0 == pCB->_MPU_Ready);         // Wait until READY
    //------------------------
    return _MPUAsyncRead(pCB, pSample);
    }
//*************************************************************


//*************************************************************
uint    MPUAsyncReadCombined(MPUData* pSample)
    {
    //---------------------------------------------------------
    // This function attempts to read both sensors (if both are
    // enabled) and returns average of measurements from both.
    // If only one sensor is enabled, this function returns
    // MPU_NOTA error code; in this case please use
    // MPUAsyncReadIfReady(...) specifying available sensor.
    //---------------------------------------------------------
    #ifndef Use_MPU1
    return MPU_NOTA;
    #endif
    #ifndef Use_MPU2
    return MPU_NOTA;
    #endif
    //---------------------------------------------------------
    MPUData Sample1;
    MPUData Sample2;
    //---------------------------------------------------------
    uint    RC;
    //---------------------------------------------------------
    if ( MPU_OK != (RC = MPUAsyncReadIfReady(1, &Sample1)) )
        return RC;
    if ( MPU_OK != (RC = MPUAsyncReadIfReady(2, &Sample2)) )
        return RC;
    //---------------------------------------------------------
    pSample->TS     =  Sample1.TS;
    pSample->Temp   = (Sample1.Temp + Sample2.Temp)*0.5;
    //---------------------------------------------------------
    VectorAdd(&Sample1.A, &Sample2.A, &pSample->A);
    VectorScale(&pSample->A, 0.5, &pSample->A);
    //---------------------------------------------------------
    VectorAdd(&Sample1.G, &Sample2.G, &pSample->G);
    VectorScale(&pSample->G, 0.5, &pSample->G);
    //---------------------------------------------------------
    return MPU_OK;
    }
//*************************************************************

//*************************************************************
uint    _MPUAsyncRead(MPU_CB* pCB, MPUData* pSample)
    {
    //----------------------------------------------
    uint        Ready_Cnt;
    _MPURawData RawData;
    //----------------------------------------------
    int         current_cpu_ipl;
    //----------------------------------------------

    //==============================================
    // Enter MPU/I2C CRITICAL SECTION
    //----------------------------------------------
    SET_AND_SAVE_CPU_IPL(current_cpu_ipl, _MPU_IL);  /* disable interrupts */
    //---------------------------------
    RawData.Temp    = pCB->_MPU_Sample.Temp;
    //---------------------------------
    RawData.AX      = pCB->_MPU_Sample.AX;
    RawData.AY      = pCB->_MPU_Sample.AY;
    RawData.AZ      = pCB->_MPU_Sample.AZ;
    //---------------------------------
    RawData.GX      = pCB->_MPU_Sample.GX;
    RawData.GY      = pCB->_MPU_Sample.GY;
    RawData.GZ      = pCB->_MPU_Sample.GZ;
    //-----------------------------------------------
    Ready_Cnt       = pCB->_MPU_Ready;
    //-----------------------------------------------
    pCB->_MPU_Ready = 0;        // Sample consumed...
    //----------------------------------------------
    // Leave MPU/I2C CRITICAL SECTION
    //==============================================
    RESTORE_CPU_IPL(current_cpu_ipl);

    //==============================================
    // Set sample timestamp
    //----------------------------------------------
    pSample->TS    = TMRGetTS();
    //==============================================
    // Adjust Sample Weight to account for multiple samples
    //----------------------------------------------
    float     Weight;
    if (Ready_Cnt > 1)
        Weight = 1.0/(float)Ready_Cnt;
    else
        Weight = 1.0;
    //----------------------------------------------
    // Process collected sample
    //----------------------------------------------
    // Temperature (C) 
    //-----------------------------------------------
    pSample->Temp = (Weight * RawData.Temp - pCB->_MPU_Temp_OffsetTo0)
                    * pCB->_MPU_Temp_Sensitivity;
    //-----------------------------------------------
    // Acceleration (G)
    //-----------------------------------------------
    VectorSet   (
                Weight * RawData.AX * pCB->_MPU_Accl_Sensitivity,
                Weight * RawData.AY * pCB->_MPU_Accl_Sensitivity,
                Weight * RawData.AZ * pCB->_MPU_Accl_Sensitivity,
                //---------
                &pSample->A
                );
    //-----------------------------------------------
    // Gyroscopes (Rad/sec)
    //-----------------------------------------------
    VectorSet   (
                Weight * RawData.GX * pCB->_MPU_Gyro_Sensitivity,
                Weight * RawData.GY * pCB->_MPU_Gyro_Sensitivity,
                Weight * RawData.GZ * pCB->_MPU_Gyro_Sensitivity,
                //---------
                &pSample->G
                );
    //-----------------------------------------------
    // Applying calibration and tuning parameters
    //-----------------------------------------------
    _MPU_ApplyCalibration(pCB, pSample);
    //-----------------------------------------------
    return MPU_OK;             // The return code was OK    
    }
//*************************************************************





