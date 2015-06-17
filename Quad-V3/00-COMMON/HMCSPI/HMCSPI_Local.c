#include "HMCSPI/HMCSPI_Local.h"

//==================================================================
uint                _HMC_Init   = 0;    // Not initialized
//------------------------------------------------------------------
byte                _HMC_IL     = 4;    // SPI default interrupt
                                        // level

//==================================================================
// HMC Sensor data normalization values
//==================================================================
// HMC Sensitivity in mGs/LSB, adjusted in the ReSet(...) routine
// NOTE: Earth magnetic field is around 0.25-0.65 Gs
float                _HMC_Gain          = 0.0;
//------------------------------------------------------------------
// MAG Hard Iron correction vector
Vector                _HMC_HardIron     =  {-22.85, -112.55, -38.64};
// MAG Soft Iron correction matrix
Vector                _HMC_SoftIron_X   =  { 1.0000, -0.0183,  0.0305};
Vector                _HMC_SoftIron_Y   =  {-0.0183,  1.0269,  0.0009};
Vector                _HMC_SoftIron_Z   =  { 0.0305,  0.0009,  1.0585};
//==================================================================



//==================================================================
// Asynchronous read support
//==================================================================
volatile    uint    _HMC_Async    = 0;  // Asynchronous read mode flag
//------------------------------------------------------------------
volatile    uint    _HMC_Ready    = 0;  // Flag indicating whether asynch-
                                        // ronous read data is ready
//------------------------------------------------------------------
_HMCRawData         _HMC_Sample;        // Asynchronous sample...
//==================================================================



