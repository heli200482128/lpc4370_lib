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
	
private:

	SGPIO_Pin m_sgpio_pin;

	bool sgpio_valid(){ return m_sgpio_pin >= SGPIO_0 &&m_sgpio_pin <= SGPIO_15; }

};


/************************************************************************/
/*  CSGpio_Slice_LPC4370                                                */
/************************************************************************/
class CSGpio_Slice_LPC4370 :public CSGpio_LPC4370
{
public:
	CSGpio_Slice_LPC4370() :CSGpio_LPC4370() {}
	~CSGpio_Slice_LPC4370(){}


//	void Config_SliceMuxReg(SMC_MatchMode match_mode, SMC_ClkCaptureMode clk_capture_mode,
//		SMC_ClkGenMode clk_gen_mode, SMC_InvOutClk inv_out_clk,
//		SMC_DataCapMode data_cap_mode, SMC_ParallelMode parallel_mode, SMC_InvQualifier inv_qualifier);
//	void Config_MuxReg();
//	void Config_OutMuxReg();


private:

	SGPIO_Slice m_sgpio_slice;

};


#endif


