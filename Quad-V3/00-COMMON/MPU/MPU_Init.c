#include "MPU\MPU_Local.h"
#include "I2C\I2C_Local.h"

//------------------------------------------------------------
// Forward declaration of _MPUInit(...) helper function
//------------------------------------------------------------
uint _MPUInit(uint MPUx);

//************************************************************
uint    MPUInit(byte RateDiv, byte DLPF,
                MPU_FS_SEL GS, MPU_AFS_SEL AS)
    {
    // MPU module depends on I2C for communication with the 
    // sensor, so we need to make sure that I2C is initialized...
    byte    IL = I2CGetIL();
    if (0 == IL)
        return I2CRC_NRDY;    // I2C Module is not initialized
    //---------------------------------------------------------
    if (_MPU_Init)
        return    MPU_OK;        // Avoid repeated initialization...
    _MPU_Init    = 1;
    //---------------------------------------------------------
    _MPU_IL    = IL;
    //---------------------------------------------------------
    // Configure INTx module associated with MPU 
    //---------------------------------------------------------
    #ifdef Use_MPU1
        MPU1_IE    = 0;        // Disable INTx Interrupt for configuration
        MPU1_IF    = 0;        // Clear INTx interrupt flag (if any)
        MPU1_IP    = _MPU_IL;  // Set INTx Interrupt Priority
        MPU1_EP    = 0;        // Trigger INTx on "raising edge"
    #endif
    #ifdef Use_MPU2
        MPU2_IE    = 0;        // Disable INTx Interrupt for configuration
        MPU2_IF    = 0;        // Clear INTx interrupt flag (if any)
        MPU2_IP    = _MPU_IL;  // Set INTx Interrupt Priority
        MPU2_EP    = 0;        // Trigger INTx on "raising edge"
    #endif

    //---------------------------------------------------------
    // Now we may configure pin associated with selected INTx
    //---------------------------------------------------------
    _MPUInitPinMap();
    //---------------------------------------------------------
    // Some of the functions of this component depends on Timer,
    // so we need to make sure that the Timer is initialized...
    //---------------------------------------------------------
    TMRInitDefault();    // Hopefully timer is already initialized
                        // so this statement will be just "NOP"

    //---------------------------------------------------------
    // Now we should configure control and initialize the sensor...
    //---------------------------------------------------------
    uint    RC    = MPU_OK;
    #ifdef Use_MPU1
        if (MPU_OK != (RC = _MPUInit(1)))
            return RC;
        if (MPU_OK != (RC = MPUReset(1, RateDiv, DLPF, GS, AS)))
            return RC;
    #endif
    #ifdef Use_MPU2
        if (MPU_OK != (RC = _MPUInit(2)))
            return RC;
        if (MPU_OK != (RC = MPUReset(2, RateDiv, DLPF, GS, AS)))
            return RC;
    #endif

    //---------------------------------------------------------
    return RC;
    }

//------------------------------------------------------------
// Implementation of _MPUInit(...) helper function
//------------------------------------------------------------
uint _MPUInit(uint MPUx)
    {
    //---------------------------------------------------------
    MPU_CB*     pCB = MPUpCB(MPUx);
    if (NULL == pCB)
        return MPU_NOTA;         // Should never happened!
    //---------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Sensor-specific settings">
    switch (MPUx)
        {
        case 1:
            //-------------------------------------------------
            // Values defined in MPU_Profile.h
            //-------------------------------------------------
            pCB->_MPU_Addr = MPU1_Addr;
            pCB->_MPU_IDCx = MPU1_IDCx;
            //-------------------------------------------------
            // Initialize Rotation Matrix - MPU-1 measurements
            // need to be rotated by -22.5 degree
            //-------------------------------------------------
            VectorSet( 0.92388,  0.38268, 0.0,    &pCB->_MPU_RM_X);
            VectorSet(-0.38268,  0.92388, 0.0,    &pCB->_MPU_RM_Y);
            //-------------------------------------------------
            // Sensor-specific temperature coefficients
            // (pre-calculated))
            //-------------------------------------------------
            VectorSet(-0.0003567,  0.0000301,  0.0001011, &pCB->_MPU_Gyro_kT);
            VectorSet(-0.0000390,  0.0004051,  0.0026829, &pCB->_MPU_Accl_kT);
            //-------------------------------------------------
            // Gyro base values vector - set in MPUCalibrateGyro(...))
            //-------------------------------------------------
            VectorSet(0.0, 0.0, 0.0,    &pCB->_MPU_Gyro_Base);
            //-------------------------------------------------
            // Sensor-specific Accelerometer calibration values
            // (pre-calculated))
            //-------------------------------------------------
            VectorSet( 0.007491,  0.011434,  0.003672, &pCB->_MPU_Accl_Base);
            //-------------------------------------------------
            VectorSet( 0.992141,  0.002031,  0.000707, &pCB->_MPU_Accl_DotX);
            VectorSet( 0.002031,  0.999274, -0.001479, &pCB->_MPU_Accl_DotY);
            VectorSet( 0.000707, -0.001479,  0.982219, &pCB->_MPU_Accl_DotZ);
            //-------------------------------------------------
            break;
           
        case 2:
            //-------------------------------------------------
            // Values defined in MPU_Profile.h
            //-------------------------------------------------
            pCB->_MPU_Addr = MPU2_Addr;
            pCB->_MPU_IDCx = MPU2_IDCx;
            //-------------------------------------------------
            // Initialize Rotation Matrix - MPU-1 measurements
            // need to be rotated by +22.5 degree
            //-------------------------------------------------
            VectorSet( 0.92388, -0.38268, 0.0,    &pCB->_MPU_RM_X);
            VectorSet( 0.38268,  0.92388, 0.0,    &pCB->_MPU_RM_Y);
            //-------------------------------------------------
            // Sensor-specific temperature coefficients
            // (pre-calculated))
            //-------------------------------------------------
            VectorSet(-0.0001542, -0.0001355, -0.0000592, &pCB->_MPU_Gyro_kT);
            VectorSet(-0.0002482,  0.0000418,  0.0023967, &pCB->_MPU_Accl_kT);
            //-------------------------------------------------
            // Gyro base values vector - set in MPUCalibrateGyro(...))
            //-------------------------------------------------
            VectorSet(0.0, 0.0, 0.0,    &pCB->_MPU_Gyro_Base);
            //-------------------------------------------------
            // Sensor-specific Accelerometer calibration values
            // (pre-calculated))
            //-------------------------------------------------
            VectorSet( 0.005182,  0.017300,  0.037316, &pCB->_MPU_Accl_Base);
            //-------------------------------------------------
            VectorSet( 0.999717,  0.002628, -0.002163, &pCB->_MPU_Accl_DotX);
            VectorSet( 0.002628,  0.995437,  0.000256, &pCB->_MPU_Accl_DotY);
            VectorSet(-0.002163,  0.000256,  0.986804, &pCB->_MPU_Accl_DotZ);
            //-------------------------------------------------
            break;
            
        default:
            return MPU_NOTA; 
        }
    // </editor-fold>
    //---------------------------------------------------------
    // <editor-fold defaultstate="collapsed" desc="Common settings">
    //=====================================================
    // Temperature offset to bring the sample to 0C
    pCB->_MPU_Temp_OffsetTo0     = -12421;
    // Temp Sensitivity: Degree C per unit of LSB
    pCB->_MPU_Temp_Sensitivity   = 0.00294117647;
    //-----------------------------------------------------
    pCB->_MPU_Gyro_Sensitivity   = 0.0; // Set in MPUReset(...)
    pCB->_MPU_Accl_Sensitivity   = 0.0; // Set in MPUReset(...)
    //=====================================================
    // Asynchronous read support
    //=====================================================
    pCB->_MPU_Async  = 0;   // Asynchronous operation flag
    pCB->_MPU_Ready  = 0;   // Flag indicating whether asynch-
                            // ronous read data is ready
    //-----------------------------------------------------
    pCB->_MPU_State  = 0;   // Step number in the asynchronous
                            // read finite automata controller
    //-----------------------------------------------------
    // Asynchronous READ buffer control
    //-----------------------------------------------------
    pCB->_MPU_BufPos = 0;   // Position in MPU read buffer
    //=====================================================
    // </editor-fold>
    return MPU_OK;
    }


