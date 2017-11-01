#include "spi_timer_sgpio.h"



CSpiMaster_Timer_SGpio::CSpiMaster_Timer_SGpio()
{
	m_wordLenght = DBIT_INVALID;
	m_clockPhase = CPHA_0;
	m_bitRateHz = 500000;

	m_clk_idle_level = false;

	m_spi_outreg_mask = 0;
	
//	m_spi_stage = SPI_BEGIN;

}


CSpiMaster_Timer_SGpio::~CSpiMaster_Timer_SGpio()
{
}

bool CSpiMaster_Timer_SGpio::Open(LPC_TIMER_T* pRegTimer, uint32_t frequency,
								CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_cs,
								CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_clk,
								CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_mosi)
{
	m_spi_outreg_mask = 0;
	
	//timer
	if (CTimer_LPC4370::Open(pRegTimer, frequency) == false)	return false;

	//cs
	if (m_sgpio_pin_cs.Open(chip_sgpio_cs, SCU_MODE_PULLUP) == false)	return false;

	m_sgpio_pin_cs.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_pin_cs.Set_GpioOenReg(true);
	m_spi_outreg_mask |= 1 << m_sgpio_pin_cs.sgpio_index();

	//clk
	if (m_sgpio_pin_clk.Open(chip_sgpio_clk, SCU_MODE_PULLUP) == false)	return false;

	m_sgpio_pin_clk.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_pin_clk.Set_GpioOenReg(true);
	m_spi_outreg_mask |= 1 << m_sgpio_pin_clk.sgpio_index();

	//mosi
	if (m_sgpio_pin_mosi.Open(chip_sgpio_mosi, SCU_MODE_PULLUP) == false)	return false;

	m_sgpio_pin_mosi.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_pin_mosi.Set_GpioOenReg(true);
	m_spi_outreg_mask |= 1 << m_sgpio_pin_mosi.sgpio_index();


	reset_signals();

	return true;
}

bool CSpiMaster_Timer_SGpio::Setup(wordSize_t wordLenght, clockPhase_t clockPhase,
									clockPolarity_t	clockPolarity, bitRate_t bitRateHz)
{
	if (m_bRunning)	return false;

	m_wordLenght = wordLenght;
	m_clockPhase = clockPhase;
	m_bitRateHz = bitRateHz;

	if (clockPolarity == CPOL_0)	m_clk_idle_level = false;
	else							m_clk_idle_level = true;


	reset_signals();

	return true; 
}


void CSpiMaster_Timer_SGpio::Close()
{
	Stop();

	//timer
	CTimer_LPC4370::Close();

	//cs
	m_sgpio_pin_cs.Close();

	//clk
	m_sgpio_pin_clk.Close();

	//mosi
	m_sgpio_pin_mosi.Close();
}

bool CSpiMaster_Timer_SGpio::Start(uint32_t data)
{
	if (SPI_DEV_VALID() == false)	return false;
	if (SPI_PARAM_VALID() == false)	return false;
	if (m_bRunning)	return false;


	m_spi_outreg.reinit();

	int8_t bit_offset = (int8_t)m_wordLenght;


	int sgpio_cs_index = m_sgpio_pin_cs.sgpio_index();
	int sgpio_clk_index = m_sgpio_pin_clk.sgpio_index();
	int sgpio_mosi_index = m_sgpio_pin_mosi.sgpio_index();

	for (m_spi_outreg.length = 0; m_spi_outreg.length < SPI_HEAD_LENGTH; m_spi_outreg.length++)
	{
		m_spi_outreg.reg[m_spi_outreg.length] |= 1 << sgpio_cs_index;
		m_spi_outreg.reg[m_spi_outreg.length] |= m_clk_idle_level << sgpio_clk_index;
	}

	int clk_offset = SPI_HEAD_LENGTH;
	int mosi_offset = SPI_HEAD_LENGTH; 
	mosi_offset += (m_clockPhase == CPHA_0) ? 0 : 2;

	while (bit_offset--)
	{
		
		//clk
		m_spi_outreg.reg[clk_offset++] |= m_clk_idle_level << sgpio_clk_index;
		m_spi_outreg.reg[clk_offset++] |= (!m_clk_idle_level) << sgpio_clk_index;
		m_spi_outreg.reg[clk_offset++] |= (!m_clk_idle_level) << sgpio_clk_index;
		m_spi_outreg.reg[clk_offset++] |= m_clk_idle_level << sgpio_clk_index;

		//mosi
		uint32_t mosi_bit = (data >> bit_offset) & 0x1;

		m_spi_outreg.reg[mosi_offset++] |= mosi_bit << sgpio_mosi_index;
		m_spi_outreg.reg[mosi_offset++] |= mosi_bit << sgpio_mosi_index;
		m_spi_outreg.reg[mosi_offset++] |= mosi_bit << sgpio_mosi_index;
		m_spi_outreg.reg[mosi_offset++] |= mosi_bit << sgpio_mosi_index;


	}

	m_spi_outreg.length += m_wordLenght << 2;

	m_spi_outreg.reg[m_spi_outreg.length - 1] |= 1 << sgpio_cs_index;


	CTimer_LPC4370::Start();
	return true;
}
void CSpiMaster_Timer_SGpio::Stop()
{
	if (m_bRunning == false)	return;

	CTimer_LPC4370::Stop();

}


inline bool CSpiMaster_Timer_SGpio::irq_handle()
{
	if (Chip_TIMER_MatchPending(m_pReg_timer, m_matchnum) == false)	
		return false;

	Chip_TIMER_ClearMatch(m_pReg_timer, m_matchnum);

	if (m_spi_outreg.offset == m_spi_outreg.length)
	{
		Stop();
		return true;
	}

	uint32_t sgpio_group_outreg = CSGpio_Pin_LPC4370::GetGroup_GpioOutReg();

	sgpio_group_outreg &= ~m_spi_outreg_mask;
	sgpio_group_outreg |= m_spi_outreg.reg[m_spi_outreg.offset++];

	CSGpio_Pin_LPC4370::SetGroup_GpioOutReg(sgpio_group_outreg);

	return true;

	////CTimer_LPC4370::irq_handle();

	//if (Chip_TIMER_MatchPending(m_pReg_timer, m_matchnum) == false)
	//	return false;

	//Chip_TIMER_ClearMatch(m_pReg_timer, m_matchnum);


	//switch (m_spi_stage)
	//{
	//case SPI_BEGIN:
	//	//init clk & mosi
	//	m_sgpio_pin_mosi.Set_GpioOutReg(false);
	//	m_sgpio_pin_clk.Set_GpioOutReg(m_clk_idle_level);
	//	//activate cs
	//	m_sgpio_pin_cs.Set_GpioOutReg(false);
	//	//move to next stage
	//	m_spi_stage = SPI_CLK_EDGE_1ST;
	//	break;
	//case SPI_CLK_EDGE_1ST:
	//	////alter clk & mosi
	//	//if (m_clockPhase == CPHA_0)
	//	//{
	//	//	m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);
	//	//	m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);
	//	//}
	//	//else
	//	//{
	//	//	m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);
	//	//	m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);
	//	//}

	//	//alter clk & mosi
	//	if (m_clockPhase == CPHA_0)	
	//		m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);

	//	m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);



	//	//move to next stage
	//	m_spi_stage = SPI_CLK_EDGE_2ND;
	//	break;
	//case SPI_CLK_EDGE_2ND:
	//	////alter clk
	//	//m_sgpio_pin_clk.Set_GpioOutReg(m_clk_idle_level);

	//	//alter clk & mosi
	//	if (m_clockPhase == CPHA_1)
	//		m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);

	//	m_sgpio_pin_clk.Set_GpioOutReg(m_clk_idle_level);

	//	//move to next stage
	//	if (m_bit_offset-- == 0)	m_spi_stage = SPI_END;
	//	else						m_spi_stage = SPI_CLK_EDGE_1ST;
	//	break;
	//case SPI_END:
	//	//deactive cs
	//	m_sgpio_pin_cs.Set_GpioOutReg(true);

	//	//stop timer to terminate spi transfer
	//	Stop();
	//	break;
	//default:
	//	break;
	//}


	//return true;
}




