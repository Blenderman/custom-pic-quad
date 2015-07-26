/* 
 * File:   RCRX.h
 * Author: AZarenin
 * Created on July 18, 2015, 11:31 PM
 */
//==============================================================================
#include "RCRXSym/RCRXSym_Local.h"
//==============================================================================
// Simulation control variables
//------------------------------------------------------------------------------
BOOL    _RCRXSym_IsArmed    = FALSE;
ulong   _RCRXSym_StartTS    = 0;    // Timestamp of simulation start (in msec.)
float   _RCRXSym_msRate;            // Conversion rate to convert Timer ticks
                                    // to milliseconds
//==============================================================================

