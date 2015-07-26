#include "MCM\MCM_Local.h"
#include <math.h>

//==============================================================================
// MCMInitF(...) and MCMInitT(...) initialize Motor Control Module
// for PPM mode. For both functions second parameter, UpTime, defines
// the time (in msec.) during which the ouput will be maintained at
// 100% on each of the channels, after which it will be dropped to 0%
// to allow ESC to capture Throttle range on each of the channels.
//==============================================================================

//==============================================================================
// Parameter "Freq" (50 <= Freq <= 400) defines PPM refresh frequency in
// Hertz; acceptable values correspond to periods from 20 to 2.5 msec.
//==============================================================================
void	MCMInitF(MCMConf frameConfig, uint Freq, uint UpTime)
	{
	if (Freq < 50)	Freq =  50;
	if (Freq > 400)	Freq = 400;
	// Calculate PPM refresh period (in msec.)
	float Period = 1000.0F / (float)Freq;
	//----------------------------------
	MCMInitT(frameConfig, Period, UpTime);
	}
//==============================================================================

//==============================================================================
// Parameter "Period" (2.5 <= Period <= 20) defines PPM refresh period
// in msec.; acceptable values correspond to frequencies in the range
// from 50 to 400 Hz.
//-----------------------------------------------------
void	MCMInitT(MCMConf frameConfig, float Period, uint UpTime)
	{
	if (Period <  2.5F)	Period =  2.5F;
	if (Period > 20.0F)	Period = 20.0F;
	//----------------------------------
	if (UpTime > 1000)
		UpTime = 1000;	// Max Up-Time (msec) for Throttle=100%
	//--------------------------------------------------
	_MCM_InitLocal(frameConfig, Period, UpTime);
	}
//==============================================================================

