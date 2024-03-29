#include "System.h"

//==================================================================
#ifndef __I2C_H
#define __I2C_H
//==================================================================
#define    I2CRC_OK        0
#define I2CRC_WCOL      1
#define I2CRC_BCOL      2
#define I2CRC_NACK      3
#define    I2CRC_TOUT      4
#define    I2CRC_OVFL      5
//--------------------
#define    I2CRC_NRDY      6       // I2C not initialized
#define    I2CRC_SBSY      7       // I2C is busy with SYNC
#define    I2CRC_ABSY      8       // I2C is busy with ASYNC
#define    I2CRC_BUSY      9       // I2C bus is busy?
//--------------------
#define    I2CRC_RQSTA    10       // Request from this subscriber already active
#define    I2CRC_QFULL    11       // Async subscription queue is full
//--------------------
#define I2CRC_NOTA     12       // Module not available
//==================================================================
#define I2CRC_MAX      12       // Highest error code from I2C
//==================================================================
#define    I2C_NACKRetry    50    // Retry count for NACK code
#define    I2C_BUSYRetry    50    // Retry count for BUSY code
//==================================================================

//==================================================================
// Following are the structures defining bits in I2Cx CONTROL and
// STATUS registers. Definition of these structures are copied from
// the header file for respective MCU provided as part of the XC16
// installation.
// The bit definitions within these registers are fairly stable from
// model to model, but if you plan to use this library it would make
// sense to compare (and, if necessary, adjust) bit definitions
// below with the one in the header file for your model of MCU.
//==================================================================
// <editor-fold defaultstate="collapsed" desc="I2CxCON">
//==================================================================
// I2C Control register
//==================================================================
// <editor-fold defaultstate="collapsed" desc="Important bits explained">
/*
bit 4 ACKEN: Acknowledge Sequence Enable bit (I2C Master mode receive operation)
        1 = Initiate Acknowledge sequence on SDAx and SCLx pins and transmit ACKDT
            data bit (hardware clear at end of master Acknowledge sequence)
        0 = Acknowledge sequence not in progress
bit 3 RCEN: Receive Enable bit (I2C Master mode)
        1 = Enables Receive mode for I2C (hardware clear at end of eighth bit
            of master receive data byte)
        0 = Receive sequence not in progress
bit 2 PEN: Stop Condition Enable bit (I2C Master mode)
        1 = Initiate Stop condition on SDAx and SCLx pins (hardware clear at end
            of master Stop sequence)
        0 = Stop condition not in progress
bit 1 RSEN: Repeated Start Condition Enable bit (I2C Master mode)
        1 = Initiate Repeated Start condition on SDAx and SCLx pins (hardware
            clear at end of master Repeated Start sequence)
        0 = Repeated Start condition not in progress
bit 0 SEN: Start Condition Enable bit (I2C Master mode)
        1 = Initiate Start condition on SDAx and SCLx pins (hardware clear at end
            of master Start sequence)
        0 = Start condition not in progress
 */
// </editor-fold>
typedef volatile struct
    {
    unsigned SEN : 1;
    unsigned RSEN : 1;
    unsigned PEN : 1;
    unsigned RCEN : 1;
    unsigned ACKEN : 1;
    unsigned ACKDT : 1;
    unsigned STREN : 1;
    unsigned GCEN : 1;
    unsigned SMEN : 1;
    unsigned DISSLW : 1;
    unsigned A10M : 1;
    unsigned IPMIEN : 1;
    unsigned SCLREL : 1;
    unsigned I2CSIDL : 1;
    unsigned : 1;
    unsigned I2CEN : 1;
    } I2C_CONBITS;
// </editor-fold>
//------------------------------------------------------------------
// <editor-fold defaultstate="collapsed" desc="I2CxSTAT">
//==================================================================
// I2C Status register
//==================================================================
// <editor-fold defaultstate="collapsed" desc="Importn bits explained">
/*
bit 15    ACKSTAT: Acknowledge Status bit
            Hardware set or clear at end of Slave or Master Acknowledge.
            1 = NACK received from slave
            0 = ACK received from slave
bit 14     TRSTAT: Transmit Status bit (I2C Master mode transmit operation)
            Hardware set at beginning of master transmission;
            hardware clear at end of slave Acknowledge.
            1 = Master transmit is in progress (8 bits + ACK)
            0 = Master transmit is not in progress
bit 10    BCL: Master Bus Collision Detect bit
            Hardware set at detection of bus collision.
            1 = A bus collision has been detected during a master operation
            0 = No collision
bit 9     GCSTAT: General Call Status bit
            Hardware set when address matches general call address;
            hardware clear at Stop detection.
            1 = General call address was received
            0 = General call address was not received
bit 8     ADD10: 10-Bit Address Status bit
            Hardware set at match of 2nd byte of matched 10-bit address;
            hardware clear at Stop detection.
            1 = 10-bit address was matched
            0 = 10-bit address was not matched
bit 7     IWCOL: Write Collision Detect bit
            Hardware set at occurrence of write to I2CxTRN register while busy
            (cleared by software).
            1 = An attempt to write the I2CxTRN register failed because the I2C module is busy
            0 = No collision
bit 6    I2COV: Receive Overflow Flag bit
            Hardware set at attempt to transfer I2CxRSR register to I2CxRCV register
            (cleared by software).
            1 = A byte was received while the I2CxRCV register is still holding the previous byte
            0 = No overflow
bit 5     D/A: Data/Address bit (I2C Slave mode)
            Hardware clear at device address match; hardware set by reception of slave
            byte or is set after the transmission is complete and the TBF flag is cleared.
            1 = Indicates that the last byte received was data
            0 = Indicates that the last byte received was a device address
bit 4     P: Stop bit
            Hardware set or clear when Start, Repeated Start or Stop detected.
            1 = Indicates that a Stop bit has been detected last
            0 = Stop bit was not detected last
bit 3     S: Start bit
            Hardware set or clear when Start, Repeated Start or Stop detected.
            1 = Indicates that a Start (or Repeated Start) bit has been detected last
            0 = Start bit was not detected last
bit 2     R/W: Read/Write Information bit (when operating as I2C slave)
            Hardware set or clear after reception of I2C device address byte.
            1 = Read: data transfer is output from slave
            0 = Write: data transfer is input to slave
bit 1     RBF: Receive Buffer Full Status bit
            Hardware set when the I2CxRCV register is written with received byte;
            hardware clear when software reads the I2CxRCV register.
            1 = Receive complete; I2CxRCV register is full
            0 = Receive not complete; I2CxRCV register is empty
bit 0     TBF: Transmit Buffer Full Status bit
            Hardware set when software writes to I2CxTRN register;
            hardware clear at completion of data transmission.
            1 = Transmit in progress; I2CxTRN register is full
            0 = Transmit complete; I2CxTRN register is empty
 */
// </editor-fold>
typedef volatile struct
    {
    unsigned TBF : 1;
    unsigned RBF : 1;
    unsigned R_W : 1;
    unsigned S : 1;
    unsigned P : 1;
    unsigned D_A : 1;
    unsigned I2COV : 1;
    unsigned IWCOL : 1;
    unsigned ADD10 : 1;
    unsigned GCSTAT : 1;
    unsigned BCL : 1;
    unsigned : 3;
    unsigned TRSTAT : 1;
    unsigned ACKSTAT : 1;
    } I2C_STATBITS;
// </editor-fold>
//==================================================================

//============================================================
// General support function for integration with other modules
//============================================================
byte    I2CGetIL();
//============================================================
// Initialization routine
//============================================================
void    I2CInit(uint IL, uint Speed);    
        // Parameter (1<=IL<=7) defines the
        // priority of I2Cx interrupt routine.
        //------------------------------------
        // Speed:
        // 0 - baud rate set at 123 kHz,
        // 1 - baud rate set at 200 kHz,
        // 2 - baud rate set at 400 KHz
        // 3 - baud rate set at   1 MHz
//============================================================
// Synchronous I2C API (visible externally) component
//============================================================
uint    I2CSyncRead(    uint    I2Cx,
                        byte    DevAddr,
                        byte    Register,
                        byte*   Buffer,
                        uint    BufLen    );
//-----------------------------------------------------
uint    I2CSyncWrite(   uint    I2Cx,
                        byte    DevAddr,
                        byte    Register,
                        byte*   Buffer,
                        uint    BuffLen );

//============================================================
// <editor-fold defaultstate="collapsed" desc="I2C Async Subscriber interface">
//============================================================
// I2C Subscriber interface
//============================================================
// Function pointer to I2C Interrupt callback routine
//------------------------------------------------------------
typedef    void (*I2CCallBack)(uint            ClientParam,
                            // Pointer to I2C CONTROL register
                            I2C_CONBITS*    pCON,
                            // Pointer to I2C STATUS register
                            I2C_STATBITS*   pSTAT,
                            // Pointer to I2C TRANSMIT register
                            vuint*          pTRN,
                            // Pointer to I2C RECEIVE register
                            vuint*          pRCV
                           );
//------------------------------------------------------------
// I2C Asyncronous Request data structure
//------------------------------------------------------------
// CallBack is the function pointer to the client routine that
//      will be called when (and IF!) the asynchronous request
//      successfully concludes.
// ClientParam is the "black-box" value provided by the client
//      while registering interrupr callback routine. Passed
//      to the respective callback routine "as-is".
//------------------------------------------------------------
typedef    struct
    {
    I2CCallBack CallBack;
    uint        ClientParam;
    } I2CAsyncRqst;
//============================================================
// </editor-fold>
//============================================================

//============================================================
// Asynchronous I2C API (visible externally) component
//============================================================
uint    I2CAsyncStart(    uint I2Cx, I2CAsyncRqst* Rqst);
//------------------------------------------------------------
// NOTE: I2CAsyncStop(...) should be called only from I2C 
// interrupt routine or its extension (Callback) procedure
//------------------------------------------------------------
static inline void  I2CAsyncStop(I2C_CONBITS*    pCON,
                                 I2C_STATBITS*    pSTAT)
    {
    //=========================================================
    if ( pCON->ACKEN || pCON->RCEN || pCON->PEN || pCON->RSEN || pCON->SEN    )
        // A bus operation is active, which will result in an interrupt
        // shortly, which will be processed in the I2CxISR routine.
        // We cannot initiate STOP on the bus...
        return;
    //--------------------------------------------------------
    // We may initiate STOP...
    //--------------------------------------------------------
    *((vuint*)pSTAT)    = 0;    // clear STATUS bits
    pCON->PEN           = 1;    // Initiate Stop on I2C bus
    //--------------------------------------------------------
    // NOTE: callback reference will be cleared in I2C 
        //       Interrupt routine after "STOP" processed
    //--------------------------------------------------------
    }
//============================================================

#endif



