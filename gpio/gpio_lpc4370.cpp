#include "gpio_lpc4370.h"



/********************************************************************************************************/
/*   CGpio_LPC4370   */
/********************************************************************************************************/


CGpio_LPC4370::CGpio_LPC4370()
:m_chip_port(INVALID_PORT_PIN),m_chip_pin(INVALID_PORT_PIN),m_gpio_port(INVALID_PORT_PIN),m_gpio_pin(INVALID_PORT_PIN)
{
}

//bool CGpio_LPC4370::gpio_config(const uint16_t &config)
//{
//	if (!argument_valid()) return false;
//
//	/* Set pin back to GPIO (on some boards may have been changed to something
//	else by Board_Init()) */
//	Chip_SCU_PinMuxSet(m_chip_port, m_chip_pin, config);
//
//	/* Configure GPIO pin as input */
//	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, m_chip_port, m_chip_pin);
//
//	///* Configure interrupt channel for the GPIO pin in SysCon block */
//	//Chip_SCU_GPIOIntPinSel(PININT_INDEX, TEST_INPUT_PORT, TEST_INPUT_PIN);
//
//	///* Configure channel interrupt as edge sensitive and falling edge interrupt */
//	//Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
//	//Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
//	//Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(PININT_INDEX));
//
//	return true;
//}

bool CGpio_LPC4370::argument_valid() 
{ 
	return !(m_chip_port == INVALID_PORT_PIN || m_chip_pin == INVALID_PORT_PIN || m_gpio_port == INVALID_PORT_PIN || m_gpio_pin == INVALID_PORT_PIN); 
}


bool CGpio_LPC4370::Open(const uint8_t &chip_port, const uint8_t &chip_pin, 
						const uint8_t &gpio_port, const uint8_t &gpio_pin, const uint16_t &config)
{
	if (argument_valid() == false)	Close();

	m_chip_port = chip_port;
	m_chip_pin = chip_pin;
	m_gpio_port = gpio_port;
	m_gpio_pin = gpio_pin;


	/* Set pin back to GPIO (on some boards may have been changed to something
	else by Board_Init()) */
	Chip_SCU_PinMuxSet(m_chip_port, m_chip_pin, config);

	/* Configure GPIO pin as input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, m_chip_port, m_chip_pin);

	return true;
}
void CGpio_LPC4370::Close()
{
	m_chip_port = INVALID_PORT_PIN;
	m_chip_pin = INVALID_PORT_PIN;
	m_gpio_port = INVALID_PORT_PIN;
	m_gpio_pin = INVALID_PORT_PIN;

}


/********************************************************************************************************/
/*   CGpioInt_LPC4370   */
/********************************************************************************************************/

inline bool CGpioInt_LPC4370::update_argument(const uint8_t &pin_int)
{
	if (m_pin_int != INVALID_PIN_INT)	return false;

	switch (pin_int)
	{
	case 0:	m_irqn_type = PIN_INT0_IRQn;	break;
	case 1:	m_irqn_type = PIN_INT1_IRQn;	break;
	case 2:	m_irqn_type = PIN_INT2_IRQn;	break;
	case 3:	m_irqn_type = PIN_INT3_IRQn;	break;
	case 4:	m_irqn_type = PIN_INT4_IRQn;	break;
	case 5:	m_irqn_type = PIN_INT5_IRQn;	break;
	case 6:	m_irqn_type = PIN_INT6_IRQn;	break;
	case 7:	m_irqn_type = PIN_INT7_IRQn;	break;

	default:	return false;
	}

	m_pin_int = pin_int;
	
	return true;
}

inline bool CGpioInt_LPC4370::gpio_init_config()
{
	if (!argument_valid()  || m_pin_int == INVALID_PIN_INT)	return false;

	/* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SCU_GPIOIntPinSel(m_pin_int, m_gpio_port, m_gpio_pin);

	/* Configure channel interrupt as edge sensitive and falling edge interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(m_irqn_type);
	//NVIC_EnableIRQ(PININT_NVIC_NAME);

	return true;
}


bool CGpioInt_LPC4370::Open(const uint8_t &chip_port, const uint8_t &chip_pin, 
							const uint8_t &gpio_port, const uint8_t &gpio_pin,
							const uint16_t &func_index, const uint8_t &pin_int)
{

	if (CGpio_LPC4370::Open(chip_port, chip_pin, gpio_port, gpio_pin, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP | func_index)) == false)
		return false;
	
	if (update_argument(pin_int) == false)
		return false;

	if (gpio_init_config() == false)
		return false;

	return true;

}

void CGpioInt_LPC4370::Close()
{
	Stop();

	CGpio_LPC4370::Close();
}

bool CGpioInt_LPC4370::Start()
{
	if (m_pin_int == INVALID_PIN_INT)	return false;

	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(m_irqn_type);

	return true;
}
void CGpioInt_LPC4370::Stop()
{
	if (m_pin_int == INVALID_PIN_INT) return;

	/* Disable Interrupt for UART channel */
	NVIC_DisableIRQ(m_irqn_type);

}



