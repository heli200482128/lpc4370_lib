#include "sgpio_lpc4370.h"


/************************************************************************/
/*  CSGpio_LPC4370                                                      */
/************************************************************************/

bool CSGpio_LPC4370::s_bInitialized = false;

void CSGpio_LPC4370::Init()
{
	if (s_bInitialized)	return;

	if (Chip_RGU_InReset(RGU_M0APP_RST))
	{
		// M0 not in reset, write back a '0'
		Chip_RGU_TriggerReset((CHIP_RGU_RST_T)(RGU_M0APP_RST | RGU_SGPIO_RST));
	}
	else
	{
		// M0 in reset, write back a '1'
		Chip_RGU_TriggerReset(RGU_SGPIO_RST);
	}

	s_bInitialized = true;
}



/************************************************************************/
/*  CSGpio_Pin_LPC4370                                                  */
/************************************************************************/


CSGpio_Pin_LPC4370::CSGpio_Pin_LPC4370():CSGpio_LPC4370()
,m_sgpio_pin(SGPIO_INVALID)
{
}
CSGpio_Pin_LPC4370::~CSGpio_Pin_LPC4370()
{
}

bool CSGpio_Pin_LPC4370::Open(const CHIP_SGPIO chip_sgpio, const uint16_t config)
{
	if (sgpio_valid())	Close();

	m_sgpio_pin = chip_sgpio.sgpio_pin;

	Chip_SCU_PinMuxSet(chip_sgpio.chip_port, chip_sgpio.chip_pin, config | chip_sgpio.func_index);

	return true;
}

void CSGpio_Pin_LPC4370::Close()
{
	if (!sgpio_valid())	return;

	m_sgpio_pin = SGPIO_INVALID;
}


/************************************************************************/
/*  CSGpio_Slice_LPC4370                                                */
/************************************************************************/


