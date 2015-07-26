/* 
 * File:   RCRX_Profile.h
 * Author: AZarenin
 * Created on July 18, 2015, 10:32 PM
 */
//==================================================================
#ifndef RCRX_PROFILE_H
#define	RCRX_PROFILE_H
//==================================================================
// In this project we use UART-capable RC receiver, which is
// connected to UART2 of the MCU using RX pin PR(I)83/RE3
//==================================================================
// Maximum length of a single UART message from RC Receiver
//------------------------------------------------------------------
#define _RCRX_Buf_Max 24
//------------------------------------------------------------------
// UART Module definitions - please update depending on the module
// 							 used in the current application.
//------------------------------------------------------------------
// UART Interrupt routines
//------------------------------------------------------------------
#define RCRXErrInterrupt	_U2ErrInterrupt
#define RCRXInterrupt       _U2RXInterrupt
//------------------------------------------------------------------
// UART Registers
//------------------------------------------------------------------
// UART Mode register
#define	UMODE			U2MODE
// UART Status register
#define	USTA			U2STA
// UART Baud Rate Generator (BRG) register
#define	UBRG			U2BRG
// UART Read buffer register
#define URXREG			U2RXREG
//------------------------------------------------------------------
// UART Module enable flag bits
#define	UEN				U2MODEbits.UEN
// UART Module Enable Operation bit
#define UARTEN			U2MODEbits.UARTEN
// UART Receive Buffer Data Available flag (Read Only)
#define	URXDA			U2STAbits.URXDA
//------------------------------------------------------------------
// UART interrupt bits
//------------------------------------------------------------------
// UART TX interrupt control bit	
#define	UTXIE			_U2TXIE
// UART RX interrupt control bit
#define	URXIE			_U2RXIE	
// UART RX Error interrupt control bit
#define	UREIE			_U2EIE	
//--------------------------------
// UART TX interrupt flag						
#define	UTXIF			_U2TXIF	
// UART RX interrupt flag
#define	URXIF			_U2RXIF	
// UART RX Error interrupt flag
#define	UREIF			_U2EIF	
//--------------------------------	
// UART TX Interrupt priority					
#define	UTXIP			_U2TXIP
// UART RX interrupt priority
#define	URXIP			_U2RXIP
// UART RX Error interrupt priority 
#define	UREIP			_U2EIP
//------------------------------------------------------------------
// UART Error flags
//------------------------------------------------------------------
//  Parity Error Status bit (Read Only)	
#define	UPERR			U2STAbits.PERR
//  Framing Error Status bit (Read Only)	
#define	UFERR			U2STAbits.FERR
//  Receiver Overrun Error Status bit (Read Only)	
#define	UOERR			U2STAbits.OERR

//------------------------------------------------------------------
// Peripheral Module Disable (PMD) register for selected UART Module
//------------------------------------------------------------------
#define	UPMD			_U2MD


//==================================================================
//************************************************************
// Helper function _SDLInitPinMap(void) implements mapping
// of UART pins using Peripheral Pin Select
//------------------------------------------------------------
// NOTE: this function defines physical configuration -
//         together with the definitions above this function
//         need to be updated for adapting this code to a
//         specific board.
//************************************************************
static inline void _RCRXInitPinMap(void)
    {
    //=========================================================
    // PIC24EP512Pxxx UARTx (RX) pin map for this application
    //--------------------------------------------------------
    //    UxRX     = PR(I)83/RE3
    //--------------------------------------------------------
    // NOTE: RE3 is multiplexed with AN27 of ADC;
    // This pin should be put into digital mode prior to
    // be used by UART. However "Init()" function puts all
    // not used analog pins into digital mode on the start.
    //=========================================================
    // Writing to RP control registers is protected by IOLOCK
    // bit in the OSCCON register. Please note that as part of
    // oscillator switch in the "Init()" routine this bit was
    // cleared, which enables writing to these register at any
    // time.
    //---------------------------------------------------------
    // Configure Receiver's pins...
    //---------------------------------------------------------
    _TRISE3     = 1;     // Configure RE3 port for INPUT
    _U2RXR      = 83;    // U2RX tied to PR(I)83/RE3
    //---------------------------------------------------------
    // Configure other UART2 pins...
    //---------------------------------------------------------    
    // UART configuration established in RCRXInit(...) leaves
    // UxRTS and UxCTS to general IO aAs we use UARTx with NO 
    // FLOW CONTROL. Also as we use UARTx ONLY to receive
    // data, the UTXEN (transmit enable) is left at 0, so
    // UxTX pin is not used by the module.
    // Thus we do not need to deal with mapping any other pins.
    //---------------------------------------------------------
    return;
    }

//==================================================================
#endif	/* RCRX_PROFILE_H */

