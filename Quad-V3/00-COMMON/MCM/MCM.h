#include "System.h"

#ifndef __MCM_H
#define __MCM_H
//==============================================================================
// MC (Motor Control) module provides PPM control signals to motors depending 
// on the provided input and parameters set at initialization.
//------------------------------------------------------------------------------
// Initialization parameters define motor configuration ("+" or "X") and timing
// acceptable by ESC. The timing defines the frequency of control updates issued
// by MCM to ESCs. The maximum frequency of updates is 400 Hz (2.5 msec period),
// which is the highest frquency at which a PPM signal could be reliably set
// with "0" encoded as 1 msec pulse and "1" (or 100%) - as a 2 msec pulse with
// 0.5 msec of "dead" space for the "1"-level signal.
// The lowest frequency is 50 Hz (20 msec period) achieved through extending the
// "dead space" between pulses.
//------------------------------------------------------------------------------
// For "+" orientation MCM outputs (1 through 4) corresponds to motors numbered
// clockwise starting with the front one. For "X" orientation motors are 
// numbered 1 through 4 starting from the front left motor.
// It is assumed that the even-numbered motors rotate counter-clockwise
// providing clockwise reaction rotation to the copter.
//------------------------------------------------------------------------------
// MCM input values values are 4 floating point numbers in the range from 0.0 to
// 1.0 representing base Throttle value and Roll, Pitch, and Yaw adjustment
// produced by the PID controller. MC module converts them into individual motor
// control values based upon the defined copter configuration and stated
// assumption regarding directions of rotation of individual motors.
// Resulting values are normalized to be within the range from 0.0 to 1.0.
//==============================================================================
// MC (Motor Control) data structure
//------------------------------------------------------------------------------
typedef struct 
	{
	//----------------------------
	float	Throttle;
	//----------------------------
	float	AdjRoll;
	float	AdjPitch;
	float	AdjYaw;
	//----------------------------
	} MCMData;
//==============================================================================

//==============================================================================
typedef enum
    {
    MCM_Plus,
    MCM_Cross
    } MCMConf;
//==============================================================================
    
//==============================================================================
// As the Output Compare module used by MCM to generate PPM signals by default
// relies on Timer2, Motor Control module initialization function besides 
// initializing OC1-OC4 also initializes Timer2 module for the requested refresh
// frequency/period.
//------------------------------------------------------------------------------
// NOTE: Please Do Not Change Timer2 settings to avoid breaking Motor Control.
// NOTE: Please issue TMRInit(...) prior to MCM initialization - otherwise 
//       Timer module will be initialized with the default IL.
//------------------------------------------------------------------------------
// MCMInitF(...) and MCMInitT(...) initialize Motor Control Module for PPM mode.
// For both functions second parameter, UpTime, defines
// the time (in msec.) during which the ouput will be maintained at
// 100% on each of the channels, after which it will be dropped to 0%
// to allow ESC to capture Throttle range on each of the channels.
//==============================================================================

//==============================================================================
// Parameter "Freq" (50 <= Freq <= 400) defines PPM refresh frequency in Hertz;
// acceptable values correspond to periods from 20 to 2.5 msec.
//------------------------------------------------------------------------------
void	MCMInitF(MCMConf frameConfig, uint Freq, uint UpTime);
//------------------------------------------------------------------------------
// Parameter "Period" (2.5 <= Period <= 20) defines PPM refresh period in msec.;
// acceptable values correspond to frequencies in the range from 50 to 400 Hz.
//------------------------------------------------------------------------------
void	MCMInitT(MCMConf frameConfig, float Period, uint UpTime);
//==============================================================================


//======================================================================
void	MCMSet(MCMData*	MCData);
//======================================================================
#endif





