#include "MPL\MPL_Local.h"

//-------------------------------------------------------
// MPL3115 Asynchronous Read operation is closely linked with
// the operation of the I2C module, thus we need to bring
// in the definition of some internal data structures.
//-------------------------------------------------------

//*************************************************************
uint        MPLAsyncStart()
    {
    if (!_MPL_Init)
        return MPL_NOTINIT;     // Not initialized...
    //---------------------------------------------------------
    if (_MPL_Async)
        return MPL_OK;          // Already started...
    //=========================================================
    _MPL_Ready    = 0;          // Discard async sample, if any
    //---------------------------------------------------------
    // Set flag indicating that Async mode enabled
    //---------------------------------------------------------
    _MPL_Async    = 1;
    //---------------------------------------------------------
    MPL_IF        = 0;            // Clear the interrupt flag
    MPL_IE        = 1;            // Enable MPL interrupt
    //---------------------------------------------------------
    // Let's set the "last read timestamp" to the current time
    // so that Async read routines may trigger interrupt if
    // maximum time for sample acquisition has expired.
    //---------------------------------------------------------
    _MPL_DataTS    = TMRGetTS();
    //---------------------------------------------------------
    // Due to the idiosyncrasy of the MPL3115, if the interrupt
    // line is asserted it stays asserted until the data is
    // read, thus "robbing" us from the rising edge trigger...
    //---------------------------------------------------------
    if ( MPL_INT_PORT )
        // Data Ready line ASSERTed
        {
        MPL_IF        = 1;  // Trigger interrupt manually -
                            // emulate rising edge
        }
    //=========================================================
    return MPL_OK;
    }
//*************************************************************


//*************************************************************
uint    MPLAsyncStop()
    {
    if (!_MPL_Init)
        return MPL_NOTINIT;    // Not initialized...
    //---------------------------------------------------------
    if (0 == _MPL_Async)
        return MPL_OK;        // Async is not active...
    //=====================================================
    // Disable ASYNC driver
    //=====================================================
    MPL_IE        = 0;        // Disable MPL interrupt line
    MPL_IF        = 0;        // Clear the interrupt flag
    //=========================================================
    // Clear ASYNC flag
    //=========================================================
    _MPL_Async    = 0;        // Asynchronous read not active...
    //---------------------------------------------------------
    _MPL_Ready    = 0;        // Discard async sample
    //---------------------------------------------------------
    return MPL_OK;            
    }
//*************************************************************


//*************************************************************
uint    MPLAsyncReadIfReady(MPLData* pSample)
    {
    if (0 == _MPL_Async)
        return MPL_NACT;            // Asynchronous read not active...
    //--------------------------------------------------
    if (0 == _MPL_Ready)
        {
        // Check for potentially "missing" interrupt
        if (TMRGetTS() - _MPL_DataTS > _MPL_MaxInt)
            // Looks like the Interrupt went missing...
            MPL_IF = 1; // Trigger interrupt manually - 
                        // emulate rising edge
        //-------------------------
        return MPL_NRDY;
        }
    //--------------------------------------------------
    return _MPLAsyncRead(pSample);
    }
//*************************************************************

//*************************************************************
uint    MPLAsyncReadWhenReady(MPLData* pSample)
    {
    if (0 == _MPL_Async)
        return MPL_NACT;            // Asynchronous read not active...
    //--------------------------------------------------
    while (0 == _MPL_Ready)         // Wait until READY
        {
        if (TMRGetTS() - _MPL_DataTS > _MPL_MaxInt)
            // Looks like the Interrupt went missing...
            {
            MPL_IF = 1;    // Trigger interrupt manually - emulate
                        // rising edge
            // We did our best - now wait for Data Ready...
            while (0 == _MPL_Ready);
            }
        else
          // Introduce some delay...
          TMRDelay(2);
        }
    //------------------------
    return _MPLAsyncRead(pSample);
    }
//*************************************************************

//*************************************************************
uint    _MPLAsyncRead(MPLData* pSample)
    {
    //----------------------------------------------
    int         current_cpu_ipl;
    //----------------------------------------------
    uint        Ready_Cnt;
    ulong       RawTS;
    long        RawData;
    //==============================================
    // Enter MPL/I2C CRITICAL SECTION
    //----------------------------------------------
    SET_AND_SAVE_CPU_IPL(current_cpu_ipl, _MPL_IL);  /* disable interrupts */
    //-----------------------------------------------
    Ready_Cnt    = _MPL_Ready;
    RawTS        = _MPL_DataTS;
    RawData      = _MPL_Data;
    //-----------------------------------------------
    _MPL_Ready   = 0;        // Sample consumed...
    _MPL_Data    = 0;        // Data cleared
    //----------------------------------------------
    RESTORE_CPU_IPL(current_cpu_ipl);
    //----------------------------------------------
    // Leave MPL/I2C CRITICAL SECTION
    //==============================================

    //==============================================
    // Timestamp and Altitude
    //-----------------------------------------------
    pSample->TS     = RawTS;
    pSample->Alt    = ((float)RawData)*0.0625;
    //==============================================
    if (Ready_Cnt > 1)
        // Find average of samples if multiple
        // samples summed together
        pSample->Alt /= (float)Ready_Cnt;
    //==============================================
    // Adjust normalized (averaged) Altitude to
    // account for Ground level
    //-----------------------------------------------
    pSample->Alt = pSample->Alt - _MPL_BaseAlt;
    //-----------------------------------------------
    return MPL_OK;             // The return code was OK    
    }
//*************************************************************



