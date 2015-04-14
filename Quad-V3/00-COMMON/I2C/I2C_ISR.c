#include "I2C\I2C_Local.h"

// Forward declaration
void I2C2Interrupt(_I2C_CB * pCB);


#ifdef _I2C_UseI2C1
//*******************************************************************
void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C1Interrupt(void)
	{
	//---------------------------------------------------------------
	_MI2C1IF	= 0; 	// Clear   I2C Master interrupt flag
	//---------------------------------------------------------------
	if (NULL == _I2C_CB1.CallBack)
		// There is no active Asynchronous subscribers - spurious interrupt
		{
		_MI2C1IE		= 0;	// Disable I2C Master interrupt
		return;
		}
	//---------------------------------------------------------------
    I2C2Interrupt(&_I2C_CB1);
	//---------------------------------------------------------------
    return;
	//===============================================================
	}
//*******************************************************************
#endif


#ifdef _I2C_UseI2C2
//*******************************************************************
void __attribute__((__interrupt__,__no_auto_psv__)) _MI2C2Interrupt(void)
	{
	//---------------------------------------------------------------
	_MI2C2IF	= 0; 	// Clear   I2C Master interrupt flag
	//---------------------------------------------------------------
	if (NULL == _I2C_CB2.CallBack)
		// There is no active Asynchronous subscribers - spurious interrupt
		{
		_MI2C2IE		= 0;	// Disable I2C Master interrupt
		return;
		}
	//---------------------------------------------------------------
    I2C2Interrupt(&_I2C_CB2);
	//---------------------------------------------------------------
	return;
	//===============================================================
	}
//*******************************************************************

//*******************************************************************
// Common interrupt handler routine
//-------------------------------------------------------------------
void I2C2Interrupt(_I2C_CB * pCB)
    {
	//---------------------------------------------------------------
	I2C_CONBITS*	pCON	= I2CpCON(pCB);
	I2C_STATBITS*	pSTAT	= I2CpSTAT(pCB);
	//---------------------------------------------------------------
    
	//===============================================================
    // <editor-fold defaultstate="collapsed" desc="Check for STOP condition on the bus">
	//===============================================================
    // Check for STOP condition on the bus
    //---------------------------------------------------------------
	if (1 == pSTAT->P)
		// STOP was issued due to completion of the Asynchronous
        // READ operation or as result of an error
		{
		//-----------------------------------------------------------
		// Indicate conclusion of the current Asynchronous request
		//-----------------------------------------------------------
        pCB->CallBack    = NULL;
        pCB->ClientParam = 0;
		//-----------------------------------------------------------
		// Check for Asynchronous queued requests
		//-----------------------------------------------------------
		int i;
		for (i = 0; i < I2CMaxAsyncQueue; i++)
			{
			if (pCB->_I2CRqstQueue[i].CallBack)
				{
				// Promote request from the queue to Active
				pCB->CallBack    = pCB->_I2CRqstQueue[i].CallBack;
                pCB->ClientParam = pCB->_I2CRqstQueue[i].ClientParam;
				// Free up queue entry
				pCB->_I2CRqstQueue[i].CallBack    = NULL;
                pCB->_I2CRqstQueue[i].ClientParam = 0;
				//---------------------------------------------------
				// Initiate Start on I2C bus
				//---------------------------------------------------
				pCON->SEN = 1;
				// NOTE: because I2C bus is being allocated to the client,
				//		 from now until the asynchronous operation completes
				//		 I2C interrupts (except for STOP and Error condi-
                //       tions) will be routed to client's callback
				//		 routine.
				//---------------------------------------------------
				return;
				}
			}
		//-----------------------------------------------------------
		// Current Asynchronous request completed and there is no
		// outstanding requests in the queue...
		//-----------------------------------------------------------
		I2CSetIE(pCB->_CB_ID, 0);   // Disable I2C Master interrupt
		//-----------------------------------------------------------
		return;
		}
    // </editor-fold>
	//===============================================================

	//===============================================================
    // <editor-fold defaultstate="collapsed" desc="Check for any ERROR condition on the bus">
	//===============================================================
	// Check for any ERROR condition on the bus
	//---------------------------------------------------------------
	if 	(
			pSTAT->ACKSTAT		// 1 = NACK received from slave
		||	pSTAT->BCL			// 1 = Master Bus Collision
		||	pSTAT->IWCOL		// 1 = Write Collision
		||	pSTAT->I2COV		// 1 = READ Overflow condition
		)
		{
		//-----------------------------------------------------------
		// Error: Terminate current ASYNC session
		// emulate I2CAsynStop(...)
		//-----------------------------------------------------------
		*(pCB->pI2C_STAT) = 0;	// clear STATUS bits
		pCON->PEN         = 1;	// Initiate Stop on I2C bus
		//-----------------------------------------------------------
		return;
		}
	//===============================================================
    // </editor-fold>
	//===============================================================
    
    
	//===============================================================
	// Normal I2Cx interrupt with the active subscriber - process
	// through the State Machine
	//===============================================================
	// Invoke callback function
	(*(pCB->CallBack))(pCB->ClientParam,
                       pCON,
                       pSTAT,
                       pCB->pI2C_TRN,
                       pCB->pI2C_RCV);
	//---------------------------------------------------------------
	return;
	//===============================================================
    }
//*******************************************************************

#endif
