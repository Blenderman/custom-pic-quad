#include "MPL\MPL_Local.h"

//=============================================================
// Synchronous READ SAMPLE API (visible externally)
//-------------------------------------------------------------
uint        MPLReadSample(MPLData* pSample)
    {
    if (!_MPL_Init)
        return MPL_NOTINIT;        // Not initialized...
    //-----------------------
    if (_MPL_Async)
        return MPL_ABSY;        // Asynchronous operation in progress...
    //*********************************************************
    uint        RC        = 0;
    long        Alt;
    //-----------------------------------------
    // Read MPL measurement!
    //-----------------------------------------
    if ( (RC = _MPLReadRawData(&Alt)) )
        return RC;                            // Error...
    //-----------------------------------------------
    // Capture measurement Timestamp
    //-----------------------------------------------
    _MPL_DataTS = TMRGetTS();
    //-----------------------------------------------
    // Report Timestamp and Altitude
    //-----------------------------------------------
    pSample->TS     = _MPL_DataTS;
    pSample->Alt    = ((float)Alt )*0.0625 - _MPL_BaseAlt;
    //-----------------------------------------------
    return    MPL_OK;
    }
//=============================================================


//=============================================================
// Synchronous READ (internal)
//-------------------------------------------------------------
// <editor-fold defaultstate="collapsed" desc="_MPLRead(...)">
uint    _MPLRead( byte  Register,
                  byte* Buffer,
                  uint  BufLen    )
    {
    if (_MPL_Async)
      return MPL_ABSY;  // Asynchronous operation in progress...
    //-------------------------------------------------------
    return    I2CSyncRead (MPL_I2Cx, 
                           MPL_Addr,
                           Register, Buffer, BufLen);
    }
// </editor-fold>
//=============================================================

//=============================================================
// Synchronous WRITE (internal)
//-------------------------------------------------------------
// <editor-fold defaultstate="collapsed" desc="_MPLWrite(...)">
uint    _MPLWrite(byte  Register, 
                  byte* Buffer,
                  uint  BufLen )
    {
    if (_MPL_Async)
        return MPL_ABSY;    // Asynchronous operation in progress...
    //-------------------------------------------------------
    return I2CSyncWrite(MPL_I2Cx, 
                        MPL_Addr,
                        Register, Buffer, BufLen);
    }
// </editor-fold>
//=============================================================

//=============================================================
// Synchronous READ of the raw sensor data
//-------------------------------------------------------------
uint    _MPLReadRawData(long* pAlt)
    {
    byte    CtrlR1    = 0;
    int     RC        = MPL_OK;
    byte    Data[6];

    // If Interrupt Port is high, this is an
    // indication that the new sample is ready
    // and we do not need to check STATUS register
    while (0 == MPL_INT_PORT);

    //----------------------------------------------
    // MPL has new measurement - read the STATUS
    // register (at 0x00), three bytes of altitude
    // (addresses from 0x01 to 0x03) and two bytes
    // of temperature data (at 0x04 and 0x05).
    // Temperature data is ignored, but had to be
    // read to reset the READY line
    //----------------------------------------------
    if ( MPL_OK != (RC = _MPLRead(0x0, Data, sizeof(Data)) ) )
        return RC;   // Error...
    //===============================================
    // Just to be sure, check STATUS
    //-----------------------------------------------
    if ( (Data[0] & 0x04) == 0x04 )
        {
        // PDR bit is set - we have a new sample; should
        // not come as surprise as we have been checking
        // MPL_INT_PORT, which, with interrupt line
        // configured in MPLReset(...), matches the
        // READY status.
        //===============================================
        // Construct 20-bit ALT sample as integer
        //-----------------------------------------------
        (*pAlt) = *((sbyte*)&Data[1]);
        (*pAlt) = (*pAlt) << 8;
        (*pAlt) = (*pAlt) |  Data[2];
        (*pAlt) = (*pAlt) << 8;
        (*pAlt) = (*pAlt) |  Data[3];
        (*pAlt) = (*pAlt) >> 4;
        }

    //---------------------------------------------------------
    // Due to the rather slow AUTO acquisition rate of MPL3115
    // (1 sec.) we always use the sensor in STANDBY + OST mode,
    // in which case OST is treated by device as an auto-clear
    // bit.
    // Prior to setting OST the sensor requires that this bit
    // is read, which is addressed by our implementation logic.
    //---------------------------------------------------------
    // Read CtrlR1 to obtain OST bit
    //---------------------------------------------------------
    if (MPL_OK != (RC = _MPLRead(CtrlR1Addr, &CtrlR1, 1)))
        return RC;    // Failure...
    //------------------------------------
    // Set OST bit and write CtrlR1
    //------------------------------------
    CtrlR1 |= CtrlR1SetOST;
    if (MPL_OK != (RC = _MPLWrite(CtrlR1Addr, &CtrlR1, 1)))
        return RC;    // Failure...
    //---------------------------------------------------------
    return    MPL_OK;
    }
//=============================================================


