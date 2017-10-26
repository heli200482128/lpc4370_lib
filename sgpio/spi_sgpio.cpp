#include "spi_sgpio.h"



CSpiMaster_SGpio::CSpiMaster_SGpio()
{
}


CSpiMaster_SGpio::~CSpiMaster_SGpio()
{
}


bool CSpiMaster_SGpio::Open(const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_cs,
							const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_clk,
							const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_mosi,
							const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_miso)
{
	if (m_cs.pin.Open(chip_sgpio_cs, SCU_MODE_PULLUP) == false)	return false;
	if (m_clk.pin.Open(chip_sgpio_clk, SCU_MODE_PULLUP) == false)	return false;
	if (m_mosi.pin.Open(chip_sgpio_mosi, SCU_MODE_PULLUP) == false)	return false;
	if (m_miso.pin.Open(chip_sgpio_miso, PLAIN_ENABLE | INBUF_ENABLE) == false)	return false;



	return true;
}


void CSpiMaster_SGpio::Close()
{
	m_cs.pin.Close();
	m_clk.pin.Close();
	m_mosi.pin.Close();
	m_miso.pin.Close();
}


