/* 
 * File:   RCRX.h
 * Author: AZarenin
 * Created on July 18, 2015, 11:31 PM
 */
//==============================================================================
#include "System.h"
//==============================================================================
#include "RCRX/RCRX.h"
//==============================================================================
#ifndef __RCRXSym_H
#define __RCRXSym_H
//==============================================================================
// Simplified ARMing for simulation
void	RCRXSymArm();						
//-----------------------------------------------------
// The following 2 function read simulated data file if
// RCControl from the receiver is 1
//-----------------------------------------------------
// Blocking call - provides new simulated sample when
// the true update obtained from receiver
uint	RCRXSymReadWhenReady(RCRXData*	RCSample);
//-----------------------------------------------------
// Non-blocking call - returns immediately sample from
// the simulated sequence based upon the time from the
// start of the simulation
uint	RCRXSymRead(RCRXData* RCSample);	
//==============================================================================
#endif		// __RCRXSym_H //





