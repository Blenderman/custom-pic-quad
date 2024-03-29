#include "TMR\TMR.h"
#include "I2C\I2C.h"
//--------------------------------------
#include "MPL\MPL.h"
#include "MPL\MPL_Profile.h"
//==================================================================

//==================================================================
#ifndef __MPL_Local_H
#define __MPL_Local_H
//==================================================================
extern volatile byte    _MPL_IL;     // MPL default interrupt level
extern volatile uint    _MPL_Init;   // Init flag
//==================================================================

//==================================================================
// Absolute altitude at the flight location (in meters)
//------------------------------------------------------------------
extern volatile float    _MPL_BaseAlt;
//==================================================================

//==================================================================
// CtrlR1 is one of the most important registers and will be written
// to quite often to initialize OST (One Shot mode). Thus we provide
// several #defines related to this register.
//==================================================================
#define CtrlR1Addr      0x26
//---------------------------
#define CtrlR1ClrOST    0xFD
#define CtrlR1SetOST    0x02


//==================================================================
// Asynchronous Operation variables' definitions
//==================================================================
extern volatile uint    _MPL_Async;    // Asynchronous operation flag
                                       // Also, I2C subscription ID
extern volatile uint    _MPL_State;    // Step number in the asynchronous
                                       // read finite automata controller

//------------------------------------------------------------------
// Asynchronous READ intermediate data storage
//------------------------------------------------------------------
// Flag indicating whether asynchronous read data is ready
extern volatile uint    _MPL_Ready;
// Status of the interrupr line at the entry to the Interrupt
// routine - used to validate Sample against interrupts generated
// due to sample time expiration.
extern volatile uint    _MPL_PortLvl;
// Asynchronous sample...
extern volatile long    _MPL_Data;    
// TS (in usec) of the last asynchronous sample
extern volatile ulong   _MPL_DataTS;
// Max interval between consequitive data samples (in usec)
// Set in the MPL_Reset Routine bases upon the value of OSR parameter
extern          ulong   _MPL_MaxInt;
//------------------------------------------------------------------
// Asynchronous READ data buffer
//------------------------------------------------------------------
extern volatile uint    _MPL_BufPos;
extern volatile byte    _MPL_Buffer[5];
extern volatile byte    _MPL_CtrlR1;
//------------------------------------------------------------------

//==================================================================
// Callback functions for I2C subscription
//------------------------------------------------------------------
void    _MPLCallBack();
//==================================================================

//==================================================================
// Asynchronous helper functions used by other asynchronous APIs
//==================================================================
// Asynchronous Read function (internal)
//------------------------------------------------------------------
uint    _MPLAsyncRead(MPLData* pSample);
//==================================================================

//==================================================================
// Synchronous helper functions used by other synchronous APIs
//==================================================================
// Synchronous READ (internal)
//------------------------------------------------------------------
uint    _MPLRead(   byte     Register,
                    byte*    Buffer,
                    uint     BufLen    );
//------------------------------------------------------------------
// Synchronous WRITE (internal)
//------------------------------------------------------------------
uint    _MPLWrite(  byte     Register, 
                    byte*    Buffer,
                    uint     BufLen );
//------------------------------------------------------------------
// Synchronous Read Raw Sample (internal)
//------------------------------------------------------------------
uint    _MPLReadRawData(long* pAlt);
//==================================================================
#endif
