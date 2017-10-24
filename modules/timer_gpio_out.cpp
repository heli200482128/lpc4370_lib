#include "timer_gpio_out.h"



CTimer_GpioOut::CTimer_GpioOut():CTimer_LPC4370(),CGpio_LPC4370()
{
	m_gpio_sequence.reinit();
}


CTimer_GpioOut::~CTimer_GpioOut()
{
	m_gpio_sequence.reinit();
}

bool CTimer_GpioOut::Init(LPC_TIMER_T* pRegTimer, uint32_t frequency,
								const uint8_t &chip_port, const uint8_t &chip_pin,
								const uint8_t &gpio_port, const uint8_t &gpio_pin, const uint16_t &config)
{
	if (CGpio_LPC4370::Open(chip_port, chip_pin, gpio_port, gpio_pin, GPIO_DIR_OUTPUT, config) == false)	return false;
	
	if (CTimer_LPC4370::Open(pRegTimer, frequency) == false)
	{
		CGpio_LPC4370::Close();
		return false;
	}
	
	return true;
}
void CTimer_GpioOut::Deinit()
{
	CTimer_LPC4370::Close();
	CGpio_LPC4370::Close();
}


bool CTimer_GpioOut::Start(bool *p_sequence, unsigned int sequence_length)
{
	if (m_bRunning)	return false;

	m_gpio_sequence.setup(p_sequence, sequence_length);

	return CTimer_LPC4370::Start();
}

void CTimer_GpioOut::Stop()
{
	CTimer_LPC4370::Stop();
	m_gpio_sequence.reinit();
}    

bool CTimer_GpioOut::irq_handle()
{
	if (CTimer_LPC4370::irq_handle() == false)	return false;

	if (m_gpio_sequence.data_offset_unused < m_gpio_sequence.data_length)
	{
		Write(m_gpio_sequence.p_data[m_gpio_sequence.data_offset_unused++]);
	}

	return true;
}
