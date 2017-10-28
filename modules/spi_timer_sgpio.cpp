#include "spi_timer_sgpio.h"



CSpiMaster_Timer_SGpio::CSpiMaster_Timer_SGpio()
{
	m_wordLenght = DBIT_INVALID;
	m_clockPhase = CPHA_0;
	m_bitRateHz = 500000;

	m_bits = 0;
	m_bit_offset = 0;

	m_clk_idle_level = false;
	
	m_spi_stage = SPI_BEGIN;

}


CSpiMaster_Timer_SGpio::~CSpiMaster_Timer_SGpio()
{
}

bool CSpiMaster_Timer_SGpio::Open(LPC_TIMER_T* pRegTimer, uint32_t frequency,
								CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_cs,
								CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_clk,
								CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_mosi)
{
	//timer
	if (CTimer_LPC4370::Open(pRegTimer, frequency) == false)	return false;

	//cs
	if (m_sgpio_pin_cs.Open(chip_sgpio_cs, SCU_MODE_PULLUP) == false)	return false;

	m_sgpio_pin_cs.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_pin_cs.Set_GpioOenReg(true);
	
	//clk
	if (m_sgpio_pin_clk.Open(chip_sgpio_clk, SCU_MODE_PULLUP) == false)	return false;

	m_sgpio_pin_clk.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_pin_clk.Set_GpioOenReg(true);

	//mosi
	if (m_sgpio_pin_mosi.Open(chip_sgpio_mosi, SCU_MODE_PULLUP) == false)	return false;

	m_sgpio_pin_mosi.Set_OutMuxCfg(OMC_GPIO_OUT, OMC_GPIO_OE);
	m_sgpio_pin_mosi.Set_GpioOenReg(true);


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

	m_bits = data;
	m_bit_offset = (int8_t)m_wordLenght - 1;
	m_spi_stage = SPI_BEGIN;

	CTimer_LPC4370::Start();
	return true;
}

//void CSpiMaster_Timer_SGpio::Stop()
//{
//	CTimer_LPC4370::Stop();
//
//	m_bit_offset = 0;
//	m_spi_stage = SPI_BEGIN;
//}


inline bool CSpiMaster_Timer_SGpio::irq_handle()
{
	CTimer_LPC4370::irq_handle();

	switch (m_spi_stage)
	{
	case SPI_BEGIN:
		//init clk & mosi
		m_sgpio_pin_mosi.Set_GpioOutReg(false);
		m_sgpio_pin_clk.Set_GpioOutReg(m_clk_idle_level);
		//activate cs
		m_sgpio_pin_cs.Set_GpioOutReg(false);
		//move to next stage
		m_spi_stage = SPI_CLK_EDGE_1ST;
		break;
	case SPI_CLK_EDGE_1ST:
		//alter clk & mosi
		if (m_clockPhase == CPHA_0)
		{
			m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);
			m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);
		}
		else
		{
			m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);
			m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);
		}

		////alter clk & mosi
		//if (m_clockPhase == CPHA_0)	
		//	m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);

		//m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);



		//move to next stage
		m_spi_stage = SPI_CLK_EDGE_2ND;
		break;
	case SPI_CLK_EDGE_2ND:
		//alter clk
		m_sgpio_pin_clk.Set_GpioOutReg(m_clk_idle_level);

		////alter clk & mosi
		//if (m_clockPhase == CPHA_1)
		//	m_sgpio_pin_mosi.Set_GpioOutReg((m_bits >> m_bit_offset) & 0x1);

		//m_sgpio_pin_clk.Set_GpioOutReg(!m_clk_idle_level);

		//move to next stage
		if (m_bit_offset-- == 0)	m_spi_stage = SPI_END;
		else						m_spi_stage = SPI_CLK_EDGE_1ST;
		break;
	case SPI_END:
		//deactive cs
		m_sgpio_pin_cs.Set_GpioOutReg(true);

		//stop timer to terminate spi transfer
		Stop();
		break;
	default:
		break;
	}


	return true;
}




