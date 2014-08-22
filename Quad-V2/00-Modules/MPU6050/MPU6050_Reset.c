#include "MPU6050/MPU6050_Local.h"


//=============================================================
uint	MPUReset(byte RateDiv, byte DLPF)
	{
	if (!_MPU_Init)
		return MPU_NOTINIT;		// Not initialized...
	//-----------------------
	if (_MPU_Async)
		return MPU_ABSY;		// Asynchronous operation in progress...
	//*********************************************************
	uint		RC	= 0;	
	//*********************************************************
	// First, for consistency, let's RESET MPU - all regs are
	// set to 0x00, except the Reg6B=0x40 (sleep)
	//---------------------------------------------------------
	byte	Reg6BReset	= 0x80;
	RC	= _MPUWrite(0x6B, &Reg6BReset, 1);
	if (RC) return RC;	// Failure...
	// Wait until RESET completes
	byte	Reg6BStat	= 0x00;
	while (0x40 != Reg6BStat)
		{
		TMRDelay(4); // ~4ms short delay...
		RC = _MPURead(0x6B, &Reg6BStat, 1);
		if (RC) return RC;	// Failure...
		}
	//---------------------------------------------------------
	// Second, we have to take MPU out of "sleep" and configure
	// gyro-based clock source to use Gyro-X
	//---------------------------------------------------------
	byte	Reg6B		= 0x01;
	RC	= _MPUWrite(0x6B, &Reg6B, 1);
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	// Now we may proceed configuring MPU
	//---------------------------------------------------------
	byte	Reg01	= 0x80;		// Aux I2C at VDD
	RC	= _MPUWrite(0x01, &Reg01, 1);	
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	// Sample Rate = Gyroscope Output Rate / (1 + RateDiv)
	// 	where Gyroscope Output Rate = 8kHz when the DLPF is 
	//	disabled (DLPF = 0 or 7), and 1kHz when the DLPF is enabled
	byte	Reg19	= RateDiv;		
	RC	= _MPUWrite(0x19, &Reg19, 1);		
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	// Digital low-pass filter: when DLPF = 0 - disabled,
	// when DLPF = 1...6 - enabled, DLPF = 7 - reserved
	byte	Reg1A	= DLPF & 0x07;	
	if (Reg1A > 6)	
		Reg1A = 6;		// Highest allowed value
	RC	= _MPUWrite(0x1A, &Reg1A, 1);		
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	byte	Reg1B	= 0x18;		// Gyro = 2000`/sec 
	RC	= _MPUWrite(0x1B, &Reg1B, 1);	
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	byte	Reg1C	= 0x00;		// Acel = +/-2g 
	RC	= _MPUWrite(0x1C, &Reg1C, 1);	
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	// Interrupt control register
	byte	Reg37	= 0b00010010;
				//---------------------------------------------
				// INT Active High		(0)
				// INT push-pull		(0)
				// INT pulse 50 usec	(0)
				// Any READ clears INT	(1)
				//---------------------
				// FSYNC disabled [3:2]	(0)
				//---------------------
				// I2C_BYPAS_EN			(1)
				// CLKOUT diabled		(0)
				//---------------------------------------------
	RC	= _MPUWrite(0x37, &Reg37, 1);	
	if (RC) return RC;	// Failure...
	//---------------------------------------------------------
	byte	Reg38	= 0x01;		// Only DATA_RDY_EN
	RC	= _MPUWrite(0x38, &Reg38, 1);	
	if (RC) return RC;	// Failure...
	
	//*********************************************************
	// Warm-up the sensors...
	//---------------------------------------------------------
	_MPURawData	RawData;
	ulong		EndTime	;
	//---------------------------------------------------------
	// Warm-up sensor for 2 second
	//---------------------------------------------------------
	EndTime	= TMRGetRTC() + 2000;	// Current time in msec offset
									// into future by 2 seconds.
	//-----------------------------
	do
		{
		if ( (RC = _MPUReadRawData(&RawData)) )	
			return RC;							// Error...
		}  
	while (TMRGetRTC() < EndTime);
	//-----------------------------
	return MPU_OK;
	}
//=============================================================

//=============================================================
uint	MPUCalibrate()
	{
	if (!_MPU_Init)
		return MPU_NOTINIT;		// Not initialized...
	//-----------------------
	if (_MPU_Async)
		return _MPUCalibrateAsync();
	else
		return _MPUCalibrateSync();
	//-----------------------
	}




