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
	if (SGPIO_VALID(chip_sgpio.sgpio_pin) == false)	return false;
	if (SGPIO_VALID(m_sgpio_pin))	return false;
	 
	m_sgpio_pin = chip_sgpio.sgpio_pin;

	Chip_SCU_PinMuxSet(chip_sgpio.chip_port, chip_sgpio.chip_pin, config | chip_sgpio.func_index);

	return true;
}

void CSGpio_Pin_LPC4370::Close()
{
	if (!SGPIO_VALID(m_sgpio_pin))	return;

	m_sgpio_pin = SGPIO_INVALID;
}

void CSGpio_Pin_LPC4370::Set_OutMuxCfg(OMC_PinOutCfg pinOutCfg, OMC_PinOeCfg pinOeCfg)
{
	LPC_SGPIO->OUT_MUX_CFG[SGPIO_INDEX(m_sgpio_pin)] = (pinOutCfg << 0) | (pinOeCfg << 4);
}

inline bool CSGpio_Pin_LPC4370::Get_GpioInReg()
{
	return ((LPC_SGPIO->GPIO_INREG & (0x1 << SGPIO_INDEX(m_sgpio_pin))) != 0);
}
void CSGpio_Pin_LPC4370::Set_GpioOutReg(const bool output)
{
	if (output)	LPC_SGPIO->GPIO_OUTREG |= 0x1 << SGPIO_INDEX(m_sgpio_pin);
	else		LPC_SGPIO->GPIO_OUTREG &= ~(0x1 << SGPIO_INDEX(m_sgpio_pin));

}
void CSGpio_Pin_LPC4370::Set_GpioOenReg(const bool enable)
{
	if (enable)	LPC_SGPIO->GPIO_OENREG |= 0x1 << SGPIO_INDEX(m_sgpio_pin);
	else		LPC_SGPIO->GPIO_OENREG &= ~(0x1 << SGPIO_INDEX(m_sgpio_pin));
}


/************************************************************************/
/*  CSGpio_Slice_LPC4370                                                */
/************************************************************************/


bool CSGpio_Slice_LPC4370::Open(const SGPIO_Slice sgpio_slice)
{
	if (SLICE_VALID(sgpio_slice) == false) return false;
	if (SLICE_VALID(m_sgpio_slice))	return false;


	m_sgpio_slice = sgpio_slice;

	return true;
}
void CSGpio_Slice_LPC4370::Close()
{
	if (!SLICE_VALID(m_sgpio_slice))	return;

	m_sgpio_slice = SGPIO_SLICE_INVALID;
}

void CSGpio_Slice_LPC4370::Set_SgpioMuxCfg(MC_ExtClkEnable 	extClkEnable,
											MC_ClkSrcPin 		clkSrcPin,
											MC_ClkSrcSlice 		clkSrcSlice,
											MC_QualMode 		qualMode,
											MC_QualPinMode 		qualPinMode,
											MC_QualSliceMode 	qualSliceMode,
											MC_ConcatEnable 	concatEnable,
											MC_ConcatOrder 		concatOrder)
{
	LPC_SGPIO->SGPIO_MUX_CFG[SLICE_INDEX(m_sgpio_slice)] = (extClkEnable << 0) | 
															(clkSrcPin << 1) | 
															(clkSrcSlice << 3) |
															(qualMode << 5) | 
															(qualPinMode << 7) | 
															(qualSliceMode << 9) |
															(concatEnable << 11) | 
															(concatOrder << 12);
}

void CSGpio_Slice_LPC4370::Set_SliceMuxCfg(SMC_MatchMode 		matchMode,
											SMC_ClkCaptureMode	clkCaptureMode,
											SMC_ClkGenMode		clkGenMode,
											SMC_InvOutClk		invOutClk,
											SMC_DataCapMode		dataCaptureMode,
											SMC_ParallelMode	parallelMode,
											SMC_InvQualifier	invQualifier)
{
	LPC_SGPIO->SLICE_MUX_CFG[SLICE_INDEX(m_sgpio_slice)] = (matchMode << 0) |
															(clkCaptureMode << 1) |
															(clkGenMode << 2) |
															(invOutClk << 3) |
															(dataCaptureMode << 4) |
															(parallelMode << 6) |
															(invQualifier << 8);
}

void CSGpio_Slice_LPC4370::Set_Reg(const uint32_t reg)
{
	LPC_SGPIO->REG[SLICE_INDEX(m_sgpio_slice)] = reg;
}
void CSGpio_Slice_LPC4370::Set_RegSS(const uint32_t reg_ss)
{
	LPC_SGPIO->REG_SS[SLICE_INDEX(m_sgpio_slice)] = reg_ss;
}
void CSGpio_Slice_LPC4370::Set_Preset(const uint16_t preset)
{
	if (preset < COUNT_RELOAD_MIN || preset > COUNT_RELOAD_MAX)	return;
	LPC_SGPIO->PRESET[SLICE_INDEX(m_sgpio_slice)] = ((uint32_t)preset & 0x1FFF) - 1;

}
void CSGpio_Slice_LPC4370::Set_Count(const uint16_t count)
{
	if (count < COUNT_RELOAD_MIN || count > COUNT_RELOAD_MAX)	return;
	LPC_SGPIO->COUNT[SLICE_INDEX(m_sgpio_slice)] = ((uint32_t)count & 0x1FFF) - 1;

}
void CSGpio_Slice_LPC4370::Set_Pos(const uint8_t pos)
{
	LPC_SGPIO->POS[SLICE_INDEX(m_sgpio_slice)] = (((uint32_t)pos - 1) << 8) | ((uint32_t)pos - 1);
}
//void CSGpio_Slice_LPC4370::Set_Mask();
//void CSGpio_Slice_LPC4370::Set_CtrlEnable();
//void CSGpio_Slice_LPC4370::Set_CtrlDisable();

//void CSGpio_Slice_LPC4370::Set_ClrEnShiftClock();
//void CSGpio_Slice_LPC4370::Set_SetEnShiftClock();
//void CSGpio_Slice_LPC4370::Set_EnableShiftClock();
//void CSGpio_Slice_LPC4370::Set_StatusShiftClock();
//void CSGpio_Slice_LPC4370::Set_ClrStatusShiftClock();
//void CSGpio_Slice_LPC4370::Set_SetStatusShiftClock();

//void CSGpio_Slice_LPC4370::Set_ClrEnExchangeClock();
//void CSGpio_Slice_LPC4370::Set_SetEnExchangeClock();
//void CSGpio_Slice_LPC4370::Set_EnableExchangeClock();
//void CSGpio_Slice_LPC4370::Set_StatusExchangeClock();
//void CSGpio_Slice_LPC4370::Set_ClrStatusExchangeClock();
//void CSGpio_Slice_LPC4370::Set_SetStatusExchangeClock();

//void CSGpio_Slice_LPC4370::Set_ClrEnPatternMatch();
//void CSGpio_Slice_LPC4370::Set_SetEnPatternMatch();
//void CSGpio_Slice_LPC4370::Set_EnablePatternMatch();
//void CSGpio_Slice_LPC4370::Set_StatusPatternMatch();
//void CSGpio_Slice_LPC4370::Set_ClrStatusPatternMatch();
//void CSGpio_Slice_LPC4370::Set_SetStatusPatternMatch();

//void CSGpio_Slice_LPC4370::Set_ClrEnInputBitMatch();
//void CSGpio_Slice_LPC4370::Set_SetEnInputBitMatch();
//void CSGpio_Slice_LPC4370::Set_EnableInputBitMatch();
//void CSGpio_Slice_LPC4370::Set_StatusInputBitMatch();
//void CSGpio_Slice_LPC4370::Set_ClrStatusInputBitMatch();
//void CSGpio_Slice_LPC4370::Set_SetStatusInputBitMatch();


