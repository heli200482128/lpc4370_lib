#ifndef __SGPIO_LPC4370_H
#define __SGPIO_LPC4370_H

#include "sgpio_18xx_43xx.h"

/************************************************************************/
/*  CSGpio_LPC4370                                                      */
/************************************************************************/
class CSGpio_LPC4370
{
public:
	CSGpio_LPC4370() { CSGpio_LPC4370::Init(); }
	~CSGpio_LPC4370(){}

	static void Init();
private:
	static bool s_bInitialized;

};


/************************************************************************/
/*  CSGpio_Pin_LPC4370                                                  */
/************************************************************************/
class CSGpio_Pin_LPC4370:public CSGpio_LPC4370
{
//type definition
#define SGPIO_INDEX(sgpio_pin) (int)(sgpio_pin)
#define SGPIO_VALID(sgpio_pin) ((sgpio_pin) >= SGPIO_0 &&(sgpio_pin) <= SGPIO_15)

public:
	typedef struct __chip_gpio
	{
		uint8_t chip_port;
		uint8_t chip_pin;
		SGPIO_Pin sgpio_pin;
		uint16_t func_index;

	}CHIP_SGPIO;

//member functions and variables
public:
	CSGpio_Pin_LPC4370();
	~CSGpio_Pin_LPC4370();

	bool Open(const CHIP_SGPIO chip_sgpio, const uint16_t config);
	void Close();

	bool isValid() { return SGPIO_VALID(m_sgpio_pin); }

	//register manipulation 
	void Set_OutMuxCfg(OMC_PinOutCfg pinOutCfg, OMC_PinOeCfg pinOeCfg);

	bool Get_GpioInReg();
	void Set_GpioOutReg(const bool output);
	void Set_GpioOenReg(const bool enable);


private:

	SGPIO_Pin m_sgpio_pin;


};


/************************************************************************/
/*  CSGpio_Slice_LPC4370                                                */
/************************************************************************/
class CSGpio_Slice_LPC4370 :public CSGpio_LPC4370
{
//type definition
#define SLICE_INDEX(sgpio_slice) (int)(sgpio_slice)
#define SLICE_VALID(sgpio_slice) ((sgpio_slice) >= SGPIO_SLICE_A &&(sgpio_slice) <= SGPIO_SLICE_P)


//member functions and variables
public:
	CSGpio_Slice_LPC4370() :CSGpio_LPC4370(),m_sgpio_slice(SGPIO_SLICE_INVALID) {}
	~CSGpio_Slice_LPC4370(){}

	bool Open(const SGPIO_Slice sgpio_slice);
	void Close();


	void Set_SgpioMuxCfg(MC_ExtClkEnable 	extClkEnable,
						MC_ClkSrcPin 		clkSrcPin,
						MC_ClkSrcSlice 		clkSrcSlice,
						MC_QualMode 		qualMode,
						MC_QualPinMode 		qualPinMode,
						MC_QualSliceMode 	qualSliceMode,
						MC_ConcatEnable 	concatEnable,
						MC_ConcatOrder 		concatOrder);
	
	void Set_SliceMuxCfg(SMC_MatchMode 		matchMode,
						SMC_ClkCaptureMode	clkCaptureMode,
						SMC_ClkGenMode		clkGenMode,
						SMC_InvOutClk		invOutClk,
						SMC_DataCapMode		dataCaptureMode,
						SMC_ParallelMode	parallelMode,
						SMC_InvQualifier	invQualifier);

	void Set_Reg(const uint32_t reg);
	void Set_RegSS(const uint32_t reg_ss);
	void Set_Preset(const uint16_t preset);
	void Set_Count(const uint16_t count);
	void Set_Pos(const uint8_t pos);
	//void Set_Mask();
	//void Set_CtrlEnable();
	//void Set_CtrlDisable();

	//void Set_ClrEnShiftClock();
	//void Set_SetEnShiftClock();
	//void Set_EnableShiftClock();
	//void Set_StatusShiftClock();
	//void Set_ClrStatusShiftClock();
	//void Set_SetStatusShiftClock();

	//void Set_ClrEnExchangeClock();
	//void Set_SetEnExchangeClock();
	//void Set_EnableExchangeClock();
	//void Set_StatusExchangeClock();
	//void Set_ClrStatusExchangeClock();
	//void Set_SetStatusExchangeClock();

	//void Set_ClrEnPatternMatch();
	//void Set_SetEnPatternMatch();
	//void Set_EnablePatternMatch();
	//void Set_StatusPatternMatch();
	//void Set_ClrStatusPatternMatch();
	//void Set_SetStatusPatternMatch();

	//void Set_ClrEnInputBitMatch();
	//void Set_SetEnInputBitMatch();
	//void Set_EnableInputBitMatch();
	//void Set_StatusInputBitMatch();
	//void Set_ClrStatusInputBitMatch();
	//void Set_SetStatusInputBitMatch();


private:

	SGPIO_Slice m_sgpio_slice;

};


#endif


