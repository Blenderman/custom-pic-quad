#include "MCM/MCM_Local.h"
//--------------------------
#include "TMR/TMR.h"
#include <math.h>

//==============================================================================
uint    _MC_Init		= 0;
MCMConf _MC_Config;
//------------------------------------------------------------------------------
uint	_MC_Base_Value	= 0;
//------------------------------------------------------------------------------
float   _MC1;
float   _MC2;
float   _MC3;
float   _MC4;
//==============================================================================

//==============================================================================
// Common initialization routine
//==============================================================================
void	_MCM_InitLocal(MCMConf frameConfig, float Period, uint UpTime)
	{
	if (_MC_Init)
		return;		// Already initialized
	//--------------------------------------------------------------------------
	_MC_Init    = 1;
    _MC_Config  = frameConfig;
	//--------------------------------------------------------------------------
	// MCM Initialization utilizes TMRDelay(...) function, so if Timer is not
    // initialized, it is a good time to initialize it now...
	//--------------------------------------------------------------------------
	TMRInitDefault();

	//==========================================================================
	// The Output Compare module by default uses Timer2; we will stay with
    // default....
	//--------------------------------------------------------------------------
	// Stop/Disable Timer2; reset all flags
	//--------------------------------------------------------------------------
	T2CON		= 0;
	// Enable Timer2 if disabled through PMD
	_T2MD		= 0;	// Timer2 module enabled
	//==========================================================================
	// Timer pre-scaler and rollover counter are calculated assuming 
    // Fcy = 64 MHz based upon specified refresh Period.
	//==========================================================================
    T2CONbits.TCKPS	= 0b10;		// Set Timer2 pre-scaler to 1:64 (tick = 1 usec)
    //-----------------------
    _MC_Base_Value	= 1000;     // 1 msec at current pre-scaler 
	//--------------------------------------------------------------------------
	// Re-set Timer2 counter
	TMR2			= 0;
	// Set Timer2 rollover threshold (MC frequency counter - number of 1 usec 
    // ticks comprising Period set in msec)
	PR2				= floorf(Period * 1000) - 1;
	//--------------------------------------------------------------------------
	// Configure Timer2 interrupts
	//--------------------------------------------------------------------------
	_T2IF 		= 0 ; 		// Clear TMR2 interrupt flag
	_T2IE 		= 0 ;   	// Disable TMR2 CPU interrupt
	//==========================================================================

	//==========================================================================
	// Output-Compare module is being used in this application to generate PPM 
    // signal to control 4 motors through respective ESCs.
	// 4 OC (Output Compare) channels are being used for Motor Control.
	//--------------------------------------------------------------------------
	// Disable OC modules for configuration and reset all control bits	
	//--------------------------------------------------------------------------
    //<editor-fold defaultstate="collapsed" desc="OCxCON1 bits definitions">
    // bit 15-14 Unimplemented: Read as ?0?
    // bit 13    OCSIDL: 
    //          Stop Output Compare x in Idle Mode Control bit
    //          1 = Output Compare x Halts in CPU Idle mode
    //          0 = Output Compare x continues to operate in CPU Idle mode
    // bit 12-10 OCTSEL<2:0>:
    //          Output Compare x Clock Select bits
    //          111 = Peripheral clock (FP)
    //          110 = Reserved
    //          101 = Reserved
    //          100 = Clock source of T1CLK is the clock source of OCx 
    //                (only the synchronous clock is supported)
    //          011 = Clock source of T5CLK is the clock source of OCx
    //          010 = Clock source of T4CLK is the clock source of OCx
    //          001 = Clock source of T3CLK is the clock source of OCx
    //          000 = Clock source of T2CLK is the clock source of OCx
    // bit 9 ENFLTC:
    //          Fault C Input Enable bit
    //          1 = Output Compare Fault C input (OCFC) is enabled
    //          0 = Output Compare Fault C input (OCFC) is disabled
    // bit 8 ENFLTB:
    //          Fault B Input Enable bit
    //          1 = Output Compare Fault B input (OCFB) is enabled
    //          0 = Output Compare Fault B input (OCFB) is disabled
    // bit 7 ENFLTA:
    //          Fault A Input Enable bit
    //          1 = Output Compare Fault A input (OCFA) is enabled
    //          0 = Output Compare Fault A input (OCFA) is disabled
    // bit 6 OCFLTC:
    //          PWM Fault C Condition Status bit
    //          1 = PWM Fault C condition on OCFC pin has occurred
    //          0 = No PWM Fault C condition on OCFC pin has occurred
    // bit 5 OCFLTB:
    //          PWM Fault B Condition Status bit
    //          1 = PWM Fault B condition on OCFB pin has occurred
    //          0 = No PWM Fault B condition on OCFB pin has occurred
    // bit 4 OCFLTA:
    //          PWM Fault A Condition Status bit
    //          1 = PWM Fault A condition on OCFA pin has occurred
    //          0 = No PWM Fault A condition on OCFA pin has occurred
    // bit 3 TRIGMODE:
    //          Trigger Status Mode Select bit
    //          1 = TRIGSTAT (OCxCON2<6>) is cleared when OCxRS = OCxTMR
    //              or in software
    //          0 = TRIGSTAT is cleared only by software
    // bit 2-0 OCM<2:0>:
    //          Output Compare Mode Select bits
    //          111 = Center-Aligned PWM mode: Output set high when 
    //                OCxTMR = OCxR and set low when OCxTMR = OCxRS
    //          110 = Edge-Aligned PWM mode: Output set high when OCxTMR = 0
    //                and set low when OCxTMR = OCxR
    //          101 = Double Compare Continuous Pulse mode: Initializes OCx pin
    //                low, toggles OCx state continuously on alternate matches
    //                of OCxR and OCxRS
    //          100 = Double Compare Single-Shot mode: Initializes OCx pin low,
    //                toggles OCx state on matches of OCxR and OCxRS once
    //          011 = Single Compare mode: Compares events with OCxR, 
    //                continuously toggles OCx pin
    //          010 = Single Compare Single-Shot mode: Initializes OCx pin high,
    //                compares event with OCxR, forces OCx pin low
    //          001 = Single Compare Single-Shot mode: Initializes OCx pin low,
    //                compares event with OCxR, forces OCx pin high
    //          000 = Output compare channel is disabled//
    //</editor-fold>
    //--------------------------------------------------------------------------
    // Setting OCxCON1 to 0 disables the module, sets clock source to Timer2,
    // and disables all fault control.
    //--------------------------------------------------------------------------
	OC1CON1 = OC2CON1 = OC3CON1 = OC4CON1 = 0;
	//--------------------------------------------------------------------------
    //<editor-fold defaultstate="collapsed" desc="OCxCON2 bits definitiona">
    // bit 15 FLTMD:
    //          Fault Mode Select bit
    //          1 = Fault mode is maintained until the Fault source is removed;
    //              the corresponding OCFLTx bit is cleared in software and a 
    //              new PWM period starts
    //          0 = Fault mode is maintained until the Fault source is removed
    //              and a new PWM period starts
    // bit 14 FLTOUT:
    //          Fault Out bit
    //          1 = PWM output is driven high on a Fault
    //          0 = PWM output is driven low on a Fault
    // bit 13 FLTTRIEN: 
    //          Fault Output State Select bit
    //          1 = OCx pin is tri-stated on Fault condition
    //          0 = OCx pin I/O state defined by FLTOUT bit on Fault condition
    // bit 12 OCINV: 
    //          OCMP Invert bit
    //          1 = OCx output is inverted
    //          0 = OCx output is not inverted
    // bit 11-9 
    //          Unimplemented: Read as ?0?
    // bit 8 OC32: 
    //          Cascade Two OCx Modules Enable bit (32-bit operation)
    //          1 = Cascade module operation is enabled
    //          0 = Cascade module operation is disabled
    // bit 7 OCTRIG: 
    //          OCx Trigger/Sync Select bit
    //          1 = Triggers OCx from source designated by SYNCSELx bits
    //          0 = Synchronizes OCx with source designated by SYNCSELx bits
    //bit 6 TRIGSTAT:
    //          Timer Trigger Status bit
    //          1 = Timer source has been triggered and is running
    //          0 = Timer source has not been triggered and is being held clear
    // bit 5 OCTRIS: 
    //          OCx Output Pin Direction Select bit
    //          1 = OCx is tri-stated
    //          0 = Output compare module drives the OCx pin    
    // bit 4-0 SYNCSEL<4:0>: 
    //          Trigger/Synchronization Source Selection bits
    //          11111 = No Sync or Trigger source for OCx
    //          11110 = INT2 pin synchronizes or triggers OCx
    //          11101 = INT1 pin synchronizes or triggers OCx
    //          11100 = Reserved
    //          11011 = ADC1 module synchronizes or triggers OCx
    //          11010 = CMP3 module synchronizes or triggers OCx
    //          11001 = CMP2 module synchronizes or triggers OCx
    //          11000 = CMP1 module synchronizes or triggers OCx
    //          10111 = IC8 module synchronizes or triggers OCx
    //          10110 = IC7 module synchronizes or triggers OCx
    //          10101 = IC6 module synchronizes or triggers OCx
    //          10100 = IC5 module synchronizes or triggers OCx
    //          10011 = IC4 module synchronizes or triggers OCx
    //          10010 = IC3 module synchronizes or triggers OCx
    //          10001 = IC2 module synchronizes or triggers OCx
    //          10000 = IC1 module synchronizes or triggers OCx
    //          01111 = Timer5 synchronizes or triggers OCx
    //          01110 = Timer4 synchronizes or triggers OCx
    //          01101 = Timer3 synchronizes or triggers OCx
    //          01100 = Timer2 synchronizes or triggers OCx (default)
    //          01011 = Timer1 synchronizes or triggers OCx
    //          01010 = No Sync or Trigger source for OCx
    //          01001 = OC9 module synchronizes or triggers OCx
    //          01000 = OC8 module synchronizes or triggers OCx
    //          00111 = OC7 module synchronizes or triggers OCx
    //          00110 = OC6 module synchronizes or triggers OCx
    //          00101 = OC5 module synchronizes or triggers OCx
    //          00100 = OC4 module synchronizes or triggers OCx
    //          00011 = OC3 module synchronizes or triggers OCx
    //          00010 = OC2 module synchronizes or triggers OCx
    //          00001 = OC1 module synchronizes or triggers OCx
    //          00000 = No Sync or Trigger source for OCx	
    // </editor-fold>
    //--------------------------------------------------------------------------
    // Setting OCxCON2 to 0 disables fault mode, sets OCx pins as non-inverted
    // and driven by OCx module (vs. tri-state), and disables OCx trigger
	//--------------------------------------------------------------------------
	OC1CON2 = OC2CON2 = OC3CON2 = OC4CON2 = 0;
	//--------------------------------------------------------------------------
	// Enable OC1-OC4 in PMD
	//--------------------------------------------------------------------------
	_OC1MD		= 0;
	_OC2MD		= 0;
	_OC3MD		= 0;
	_OC4MD		= 0;
	//--------------------------------------------------------------------------
	// Enabling OCx in PMD sets control registers to their default values,
    // which includes setting SYNCSEL (OCxCON2) to 0b01100; this default
    // setting establishes Timer2 as synchronization source for OC1-OC4
	//--------------------------------------------------------------------------
    // Now that OCx modules are enabled in PMD, we may assign output pins
	//--------------------------------------------------------------------------
    _MCM_InitPinMap();
	//--------------------------------------------------------------------------
    
	//--------------------------------------------------------------------------
    // In this application we do not use Output Compare interrupts so we need
    // to reset any outstanding requests and block OC interrupts.
	//--------------------------------------------------------------------------
    _OC1IE = _OC2IE = _OC3IE = _OC4IE = 0;    
    _OC1IF = _OC2IF = _OC3IF = _OC4IF = 0;    
    _OC1IP = _OC2IP = _OC3IP = _OC4IP = 0;    
	//==========================================================================

	//==========================================================================
	// Configure OC modules:
	//--------------------------------------------------------------------------
    // Timer2 is already selected as clock source for OC1-OC4 by OCxCON1=0
	//--------------------------------------------------------------------------
	// Put all OC modules into the Edge-Aligned PWM mode
	//--------------------------------------------------------------------------
	OC1CON1bits.OCM     = 0b110;	// Edge-Aligned PWM mode on OC1
	OC2CON1bits.OCM     = 0b110;	// Edge-Aligned PWM mode on OC2
	OC3CON1bits.OCM     = 0b110;	// Edge-Aligned PWM mode on OC3
	OC4CON1bits.OCM     = 0b110;	// Edge-Aligned PWM mode on OC4
	//==========================================================================

	//==========================================================================
	// Set initial duty cycle (OCxR) registers for PPM mode.
	//--------------------------------------------------------------------------
	if (UpTime > 0) 
		{
        //----------------------------------------------------------------------
		// Special case: start with 100% motor control output so that ESC can
        // capture "high" throttle
        //----------------------------------------------------------------------
		uint MaxValue	= 2 * _MC_Base_Value;
		//-----------------------------------
		OC1R = OC2R = OC3R = OC4R = MaxValue;
		}
    else
        {
        //----------------------------------------------------------------------
        // Normal start: Set Motor Control to 0% 
        //----------------------------------------------------------------------
        OC1R = OC2R = OC3R = OC4R = _MC_Base_Value;
        //----------------------------------------------------------------------
        }

	//==========================================================================
	// Start OC1-OC4 by enabling Timer2, which provides both clock source and
    // synchronization source for selected Output Compare modules
	//--------------------------------------------------------------------------
	T2CONbits.TON 	= 0b1;
	//==========================================================================
    
	if (UpTime > 0) 
		{
        //----------------------------------------------------------------------
        // Keep output HIGH for defined UpTime
        //----------------------------------------------------------------------
		TMRDelay(UpTime);        
        //----------------------------------------------------------------------
        // Reset Motor Control to 0% 
        //----------------------------------------------------------------------
        OC1R = OC2R = OC3R = OC4R = _MC_Base_Value;
        //----------------------------------------------------------------------
		// Wait until ESC captures Low Throttle Value
        //----------------------------------------------------------------------
		TMRDelay(UpTime);
		}
	}
//==============================================================================
