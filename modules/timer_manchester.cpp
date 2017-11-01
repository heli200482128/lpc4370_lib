#include "timer_manchester.h"



CTimer_Manchester::CTimer_Manchester():CTimer_LPC4370()
{
	m_manchester_outreg_mask = 0;

	m_manchester_outreg.reinit();
}


CTimer_Manchester::~CTimer_Manchester()
{
	m_manchester_outreg.reinit();
}

bool CTimer_Manchester::Open(LPC_TIMER_T* pRegTimer, uint32_t frequency,
							const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_V1, 
							const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_V2)
{ 
	m_manchester_outreg_mask = 0;

	//V1&V2
	if (m_sgpio_V1.Open(chip_sgpio_V1, SCU_MODE_PULLUP) == false)	return false;
	m_sgpio_V1.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_V1.Set_GpioOenReg(true);
	m_manchester_outreg_mask |= 1 << m_sgpio_V1.sgpio_index();

	if (m_sgpio_V2.Open(chip_sgpio_V2, SCU_MODE_PULLUP) == false)	return false;
	m_sgpio_V2.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_V2.Set_GpioOenReg(true);
	m_manchester_outreg_mask |= 1 << m_sgpio_V2.sgpio_index();

	//timer
	if (CTimer_LPC4370::Open(pRegTimer, frequency) == false)	return false;
	

	return true;
}

void CTimer_Manchester::Close()
{
	CTimer_LPC4370::Close();

	m_sgpio_V1.Close();
	m_sgpio_V2.Close();
}


bool CTimer_Manchester::Start(bool *p_sequence, unsigned int sequence_length)
{
	if (m_bRunning)	return false;


	m_manchester_outreg.reinit();

	int sgpio_V1_index = m_sgpio_V1.sgpio_index();
	int sgpio_V2_index = m_sgpio_V2.sgpio_index();

	//v1 output 0, v2 output 1 -> manchester=1
	//v1 output 1, v2 output 0 -> manchester=0

	//close switch when timer is unstable
	for (m_manchester_outreg.length = 0; m_manchester_outreg.length < MANCHESTER_HEAD_LENGTH; m_manchester_outreg.length++)
	{
		m_manchester_outreg.reg[m_manchester_outreg.length] |= 1 << sgpio_V2_index;
	}

	for (uint32_t iSeq = 0; iSeq < sequence_length; iSeq++)
	{
		if (p_sequence[iSeq])	m_manchester_outreg.reg[m_manchester_outreg.length++] |= 1 << sgpio_V1_index;
		else					m_manchester_outreg.reg[m_manchester_outreg.length++] |= 1 << sgpio_V2_index;
	}

	//close switch after manchester sequence
	m_manchester_outreg.reg[m_manchester_outreg.length++] |= 1 << sgpio_V2_index;


	return CTimer_LPC4370::Start();
}

void CTimer_Manchester::Stop()
{
	if (m_bRunning == false)	return;
	CTimer_LPC4370::Stop();
}    

//inline bool CTimer_Manchester::irq_handle()
//{
//
//	CTimer_LPC4370::irq_handle();
//	
//	
//	if (m_manchester_outreg.data_offset_unused == m_manchester_outreg.data_length)
//	{
//		Stop();
//		return true;
//	}
//
//	uint32_t sgpio_group_outreg = CSGpio_Pin_LPC4370::GetGroup_GpioOutReg();
//
//	if (m_manchester_outreg.p_data[m_manchester_outreg.data_offset_unused++])
//	{
//		sgpio_group_outreg |= 1 << m_sgpio_index_V1;
//		sgpio_group_outreg &= ~(1 << m_sgpio_index_V2);
//	}
//	else
//	{
//		sgpio_group_outreg &= ~(1 << m_sgpio_index_V1);
//		sgpio_group_outreg |= 1 << m_sgpio_index_V2;
//	}
//	
//	CSGpio_Pin_LPC4370::SetGroup_GpioOutReg(sgpio_group_outreg);
//
//	return true;
//}
