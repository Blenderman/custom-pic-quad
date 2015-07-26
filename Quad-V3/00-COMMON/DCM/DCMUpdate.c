#include "DCM/DCMLocal.h"
#include "TMR/TMR.h"

#include <math.h>

//==================================================================================
uint		DCMUpdate(	ulong		GyroTS,	// Gyro timestamp
						Vector*		Gyro,   // Gyro measurement
                        //--------------------------------------
                        ulong		AccTS,	// Acc timestamp
						Vector*		Acc,    // Acc measurement
                        //--------------------------------------
                        ulong		MagTS,  // Mag timestamp
						Vector*		Mag,    // Mag measurement
                        //--------------------------------------
						DCMData*	IMUResult)
	{
	//==========================================================================
	if (_DCMInit != 1)
		return DCM_NOTINIT;	
	//==========================================================================
    // Calculate integration time interval for respective sensors
    //--------------------------------------------------------------------------
	float	GyroDT	= TMRGetTSRate() * (GyroTS - _DCM_GyroTS);
	float	AccDT	= TMRGetTSRate() * (AccTS  - _DCM_AccTS);
	float	MagDT	= TMRGetTSRate() * (MagTS  - _DCM_MagTS);
    //--------------------------------------------------------------------------
    // Update last sample timestamps for respective sensors
    //--------------------------------------------------------------------------
    _DCM_GyroTS     = GyroTS;
    _DCM_AccTS		= AccTS;
    _DCM_MagTS		= MagTS;
	//==========================================================================
    
	//==========================================================================
	// Accelerometer-based and Magnetometer-based rotation correction vectors
	//--------------------------------------------------------------------------
	Vector			DriftComp;		// Current step total drift compensation
    VectorSet(0.0, 0.0, 0.0, &DriftComp);
	//--------------------------------------------------------------------------
    if (TRUE == _DCM_UseAcc)
        {
        // <editor-fold defaultstate="collapsed" desc="Calculate Acc-based correction">
        //--------------------------------------------------------------------------
        // Local variables used in calculations of the Accelerometer-based correction
        //--------------------------------------------------------------------------
        Vector			EarthZ;		// Earth vertical in the Body frame
        Vector			AccNorm;	// Normalized Accelerometer measurement
                                    // in the Body frame
        Vector			AccError;	// Rotational "error" between the Earth
                                    // vertical (in Body frame) and 
                                    // acceleration vector.
        Vector			AccPterm;	// Proportional term
        Vector			AccIterm;	// Current step component of Integral term
        //--------------------------------------------------------------------------
        // Calculating Accelerometer-based correction
        //--------------------------------------------------------------------------	
        // Extract Z-axis in the Earth frame as seen from the Body frame
        // from the DCM
        DCMEarthZ(&EarthZ);        
        //---------------------------------------------------------
        // We have to "normalize" gravity vector so that calculated
        // error proportional to rotation error and independent of
        // the level of current acceleration
        //---------------------------------------------------------
        VectorNormalize(Acc, &AccNorm);
        //---------------------------------------------------------
        // Cross-product of the axis Z in the Earth frame (as seen
        // from the Body frame) of the DCM with the normalized
        // Accelerometer measurement (true Z-axis of Earth frame
        // in the Body frame ignoring linear accelerations) is the
        // ERROR accumulated in DCM and defines how much the Rotation 
        // Matrix need to be rotated so that these vectors match.
        //---------------------------------------------------------
        VectorCrossProduct(&EarthZ, &AccNorm, &AccError);
        //---------------------------------------------------------
        // Accelerometer correction P-Term
        //---------------------------------------------------------
        VectorScale(&AccError,  _DCMAccKp,              &AccPterm);	
        //---------------------------------------------------------
        // Accelerometer correction I-Term
        //---------------------------------------------------------
        VectorScale(&AccError, (_DCMAccKi * AccDT),  &AccIterm);
        VectorAdd(&_DCMAccIterm, &AccIterm, &_DCMAccIterm);
        //---------------------------------------------------------
        // Adjust drift compensation vector with Accelerometer-
        // based correction factors
        //---------------------------------------------------------
        VectorAdd(&DriftComp, &AccPterm,     &DriftComp);
        VectorAdd(&DriftComp, &_DCMAccIterm, &DriftComp);
    	//--------------------------------------------------------------------------
        // </editor-fold>
        }
	//--------------------------------------------------------------------------
    if (TRUE == _DCM_UseMag)
        {
        // <editor-fold defaultstate="collapsed" desc="Calculate Mag-based correction">
        //--------------------------------------------------------------------------
        // Local variables used in calculations of the Magnetometer-based correction
        //--------------------------------------------------------------------------
        Vector			EarthMag;	// Magnetic vector measurement
                                    // transformed to Earth frame using
                                    // current rotation matrix
        Vector			MagNorm;	// Normalized Magnetometer measurement
                                    // in the Earth frame
        Vector			MagError;	// Rotational "error" between rotated
                                    // magnetic vector (in Body frame) and 
                                    // measured magnetic vector.
        Vector			MagPterm;	// Proportional term
        Vector			MagIterm;	// Current step component of Integral term
        //--------------------------------------------------------------------------
        // Calculating Magnetometer-based correction
        //--------------------------------------------------------------------------	
        // Transform magnetic vector measurement to Earth frame
        DCMToEarth(Mag, &EarthMag);
        //---------------------------------------------------------
        // We have to "normalize" magnetic vector so that calculated
        // error proportional to rotation error and independent of
        // the strength of magnetic field
        //---------------------------------------------------------
        VectorNormalize(&EarthMag, &MagNorm);
    	//------------------------------------------------------------------
        // If magnetic vector is used ONLY for Yaw drift compensation we 
        // should nullify Z-axis component (Z-axis component of the reference
        // vector _DCM_BaseMAG was nullified in DCMReset(...) )
        //------------------------------------------------------------------
        if (TRUE == _DCM_UseMagYawOnly)
            MagNorm.Z = 0.0;    
    //---------------------------------------------------------
        // Cross-product of the original magnetic vector transformed
        // to Body frame using the DCM with the normalized
        // magnetometer measurement in the Body frame is the ERROR 
        // accumulated in DCM and defines how much the Rotation 
        // Matrix need to be rotated so that these vectors match.
        //---------------------------------------------------------
        VectorCrossProduct(&MagNorm, &_DCM_BaseMAG, &MagError);
        //---------------------------------------------------------
        // Magnetometer correction P-Term
        //---------------------------------------------------------
        VectorScale(&MagError,  _DCMMagKp,              &MagPterm);	
        //---------------------------------------------------------
        // Magnetometer correction I-Term
        //---------------------------------------------------------
        VectorScale(&MagError, (_DCMMagKi * MagDT),  &MagIterm);
        VectorAdd(&_DCMMagIterm, &MagIterm, &_DCMMagIterm);
        //---------------------------------------------------------
        // Adjust drift compensation vector with Magnetometer-
        // based correction factors
        //---------------------------------------------------------
        VectorAdd(&DriftComp, &MagPterm,     &DriftComp);
        VectorAdd(&DriftComp, &_DCMMagIterm, &DriftComp);
    	//--------------------------------------------------------------------------
        // </editor-fold>
        }
	//==========================================================================

	//==========================================================================
	// Local variables used in "rotating" Rotational Matrix
	//--------------------------------------------------------------------------
	Vector  RotationDelta;
	Matrix  SmallRotation;
	Matrix  Rotated;
	//--------------------------------------------------------------------------
	// Calculating total adjustment and "rotate" Rotational Matrix
	//--------------------------------------------------------------------------
	// Calculate rotation delta in body frame in radians through "integration"
	// of the gyro rotation rates
	VectorScale(	Gyro, GyroDT, &RotationDelta);
	//--------------------------------------------------------------------------
	VectorAdd(&RotationDelta, &DriftComp, &RotationDelta);
	// Construct "infinitesimal" rotation matrix
	MatrixSetSmallRotation(&RotationDelta, &SmallRotation);
	// Rotate DCM by "small rotation"
	MatrixMult(&_DCMRM, &SmallRotation, &Rotated);
	// Normalize and store current DCM Rotation Matrix
	_DCMNormalize(&Rotated, &_DCMRM);
	//==========================================================================

	//--------------------------------------------------------------------------
	// Load Current orientation parameters into DCMData
	//--------------------------------------------------------------------------
	IMUResult->Roll     = _DCMRoll(&_DCMRM);
	IMUResult->Pitch	= _DCMPitch(&_DCMRM);
	IMUResult->Yaw		= _DCMYaw(&_DCMRM);
	//------------------------------------------------------------
	IMUResult->Incl     = _DCMIncl(&_DCMRM);
	//--------------------------------------------------------------------------
	IMUResult->Azimuth	= DCMRangeTo2Pi(_DCM_BaseAzimuth + IMUResult->Yaw);
	//--------------------------------------------------------------------------
	IMUResult->TS       = TMRGetTS();
	//--------------------------------------------------------------------------
	return	DCM_OK;
	}
//==============================================================================
