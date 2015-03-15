<<<<<<< HEAD
#include "BLI\BLILocal.h"
#include "TMR\TMRLocal.h"
=======
#include "BLI/BLILocal.h"
#include "TMR/TMRLocal.h"
>>>>>>> Kopelow


//************************************************************
void	BLIInit()
	{
	if (_BLIInit)
		return;					// BLI has been initialized
	//----------------------------------------------------
	_BLIInit			= 1;	// BLI Initialized
	//----------------------------------------------------

	//*******************************************************************
	// BLI Component relies on Timer functions, so if Timer is not
	// initialized, it is a good time to initialize it now...
	//-------------------------------------------------------------------
	TMRInitDefault();

	//*******************************************************************
	// LED / Buzzer control port
	//-------------------------------------------------------------------
	// Configure RA10 as OUTPUT
	_TRISA10	= 0;
	_LATA10		= 0;	// Reset port
	//-------------------------------------------------------------------

	return;
	}
//************************************************************

