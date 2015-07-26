#include "MPU\MPU_Local.h"

//=============================================================
// Byte-wise Synchronous Get/Put functions
//=============================================================
uint    MPUGetByte(uint MPUx, byte Address, byte* Value)
    {
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)    return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    return _MPURead(pCB, Address, Value, 1);
    }
//-----------------------------------------------------
uint    MPUSetByte(uint MPUx, byte Address, byte  Value)
    {
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)    return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    byte Temp = Value;
    return _MPUWrite(pCB, Address, &Temp, 1);
    }
//=============================================================

//=============================================================
// Synchronous READ SAMPLE API (visible externally)
//-------------------------------------------------------------
uint        MPUReadSample(uint MPUx, MPUData* pSample)
    {
    if (!_MPU_Init)
        return MPU_NOTINIT;        // Not initialized...
    //---------------------------------------------------------
    MPU_CB*        pCB = MPUpCB(MPUx);
    if (NULL == pCB)    return MPU_NOTA;    // Should never happened!
    //------------------------------------------------------------------
    if (pCB->_MPU_Async)
        return MPU_ABSY;        // Asynchronous operation in progress...
    //*********************************************************
    uint            RC        = 0;
    _MPURawData     RawData;
    //-----------------------------------------
    // Read MPU measurement!
    //-----------------------------------------
    if ( (RC = _MPUReadRawData(pCB, &RawData)) )
        return RC;                            // Error...
    //-----------------------------------------------
    // Sample Timestamp
    //-----------------------------------------------
    pSample->TS     = TMRGetTS();
    //-----------------------------------------------
    // Temperature (C) 
    //-----------------------------------------------
    pSample->Temp = (RawData.Temp - pCB->_MPU_Temp_OffsetTo0) 
                    * pCB->_MPU_Temp_Sensitivity;
    //-----------------------------------------------
    // Acceleration (G)
    //-----------------------------------------------
    VectorSet    (
                RawData.AX * pCB->_MPU_Accl_Sensitivity,
                RawData.AY * pCB->_MPU_Accl_Sensitivity,
                RawData.AZ * pCB->_MPU_Accl_Sensitivity,
                //---------
                &pSample->A
                );
    //-----------------------------------------------
    // Gyroscopes (Rad/sec)
    //-----------------------------------------------
    VectorSet   (
                RawData.GX * pCB->_MPU_Gyro_Sensitivity,
                RawData.GY * pCB->_MPU_Gyro_Sensitivity,
                RawData.GZ * pCB->_MPU_Gyro_Sensitivity,
                &pSample->G
                );
    //-----------------------------------------------
    // Applying calibration and tuning parameters
    //-----------------------------------------------
    _MPU_ApplyCalibration(pCB, pSample);
    //-----------------------------------------------
    return    MPU_OK;
    }
//=============================================================


// <editor-fold defaultstate="collapsed" desc="Synchronous read helper functions">
//=============================================================
// Synchronous READ (internal)
//-------------------------------------------------------------
uint    _MPURead(    MPU_CB*    pCB,
                    byte     Register,
                    byte*    Buffer,
                    uint      BufLen    )
    {
    if (!_MPU_Init)
        return MPU_NOTINIT;        // Not initialized...
    //-----------------------
    if (pCB->_MPU_Async)
        return MPU_ABSY;        // Asynchronous operation in progress...
    //*********************************************************
    uint    RC        = MPU_OK;
    uint    Count    = 0;
    //---------------------------------------------------------
RetryNACK:
    RC = I2CSyncRead(pCB->_MPU_IDCx, pCB->_MPU_Addr, Register, Buffer, BufLen);
    switch (RC)
        {
        case I2CRC_OK:
            return MPU_OK;

        case I2CRC_NACK:
            Count++;
            if (Count < I2C_NACKRetry)
                goto RetryNACK;

        default:
            return    RC;
        }
    }
//=============================================================

//=============================================================
// Synchronous WRITE (internal)
//-------------------------------------------------------------
uint    _MPUWrite(    MPU_CB*     pCB,
                    byte     Register,
                    byte*     Buffer,
                    uint     BufLen )
    {
    if (!_MPU_Init)
        return MPU_NOTINIT;        // Not initialized...
    //-----------------------
    if (pCB->_MPU_Async)
        return MPU_ABSY;        // Asynchronous operation in progress...
    //*********************************************************
    uint    RC        = MPU_OK;
    uint    Count    = 0;
    //---------------------------------------------------------
RetryNACK:
    RC = I2CSyncWrite(pCB->_MPU_IDCx, pCB->_MPU_Addr, Register, Buffer, BufLen);
    switch (RC)
        {
        case I2CRC_OK:
            return MPU_OK;

        case I2CRC_NACK:
            Count++;
            if (Count < I2C_NACKRetry)
                goto RetryNACK;

        default:
            return    RC;
        }
    }
//=============================================================

//=============================================================
// Synchronous READ of the raw sensor data
//-------------------------------------------------------------
uint    _MPUReadRawData(MPU_CB*    pCB, _MPURawData* pData)
    {
    int        RC        = 0;
    byte    Status    = 0;
    ulong    Alarm;
    //-----------------------------------
    if ( (RC = _MPURead(pCB, MPU6050_INT_STATUS, &Status, 1)) )
        return RC;                            // Error...
    //-----------------------------------
    Alarm = TMRSetAlarm(50);    // Expecting to get sample within 50 msec
    //-----------------------------------
    while ( (Status & 0x01) != 0x01 )
        {
        TMRDelayTicks(1);  // Short delay...
        if ( (RC = _MPURead(pCB, MPU6050_INT_STATUS, &Status, 1)) )
            return RC;                        // Error...
        if (TMRCheckAlarm(Alarm))
            return MPU_TMOUT;
        }

    //-----------------------------------------
    // MPU has a measurement!
    //-----------------------------------------
    byte    Data[14];
    // Read measurement
    if ( (RC = _MPURead(pCB, MPU6050_DATA_START, Data, 14) ) )
        return RC;                            // Error...
    //===============================================
    // Process sensor data
    _MPU_ConvertReadBuffer(Data, pData);
    //===============================================
    return    MPU_OK;
    }
//=============================================================
// </editor-fold>

