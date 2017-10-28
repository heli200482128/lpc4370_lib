#include "gpio_lpc4370.h"



/********************************************************************************************************/
/*   CGpio_LPC4370   */
/********************************************************************************************************/


CGpio_LPC4370::CGpio_LPC4370()
:m_gpio_port(INVALID_PORT_PIN), m_gpio_pin(INVALID_PORT_PIN)
{
}


//bool CGpio_LPC4370::gpio_valid()
//{ 
//	return !(m_gpio_port == INVALID_PORT_PIN || m_gpio_pin == INVALID_PORT_PIN);
//}
//bool CGpio_LPC4370::argument_valid(CHIP_GPIO chip_gpio)
//{
//	return !(chip_gpio.chip_port == INVALID_PORT_PIN || chip_gpio.chip_pin == INVALID_PORT_PIN ||
//			chip_gpio.gpio_port == INVALID_PORT_PIN || chip_gpio.gpio_pin == INVALID_PORT_PIN);
//}


//bool CGpio_LPC4370::Open(const uint8_t chip_port, const uint8_t chip_pin, 
//						const uint8_t gpio_port, const uint8_t gpio_pin, 
//						const bool output, const uint16_t config)
//{
//	if (argument_valid() == false)	Close();
//
//	m_chip_port = chip_port;
//	m_chip_pin = chip_pin;
//	m_gpio_port = gpio_port;
//	m_gpio_pin = gpio_pin;
//
//
//	Chip_GPIO_Init(LPC_GPIO_PORT);
//
//	/* Set pin back to GPIO (on some boards may have been changed to something
//	else by Board_Init()) */
//	Chip_SCU_PinMuxSet(m_chip_port, m_chip_pin, config);
//
//	/* Configure GPIO pin's direction */
//	Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, m_gpio_port, m_gpio_pin, output);
//	//Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, m_gpio_port, m_gpio_pin);
//
//	return true;
//}

bool CGpio_LPC4370::Open(const CHIP_GPIO chip_gpio, const bool output, const uint16_t config)
{
	if (GPIO_VALID())	return false;
	
	m_gpio_port = chip_gpio.gpio_port;
	m_gpio_pin = chip_gpio.gpio_pin;

		
	Chip_GPIO_Init(LPC_GPIO_PORT);
	
	/* Set pin back to GPIO (on some boards may have been changed to something
	else by Board_Init()) */
	Chip_SCU_PinMuxSet(chip_gpio.chip_port, chip_gpio.chip_pin, config | chip_gpio.func_index);
	
	/* Configure GPIO pin's direction */
	Chip_GPIO_SetPinDIR(LPC_GPIO_PORT, chip_gpio.gpio_port, chip_gpio.gpio_pin, output);
	//Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, m_gpio_port, m_gpio_pin);
	
	return true;
}

void CGpio_LPC4370::Close()
{
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

inline bool CGpioInt_LPC4370::gpio_init_config(const GPIO_INT_TYPE gpio_int_type)
{
	if (GPIO_VALID() == false || m_pin_int == INVALID_PIN_INT)	return false;

	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	/* Configure interrupt channel for the GPIO pin in SysCon block */
	Chip_SCU_GPIOIntPinSel(m_pin_int, m_gpio_port, m_gpio_pin);

	/* Configure channel interrupt */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
	switch (gpio_int_type)
	{
	case GPIO_INT_EDGE_RISING:	// edge sensitive and rising edge interrupt
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		break;
	case GPIO_INT_EDGE_FALLING:	// edge sensitive and falling edge interrupt
		Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		break;
	case GPIO_INT_LEVEL_HIGH:	// level sensitive and high level interrupt
		Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		break;
	case GPIO_INT_LEVEL_LOW:	// level sensitive and low level interrupt
		Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));
		break;
	default:
		break;
	}

	/* Enable interrupt in the NVIC */
	NVIC_ClearPendingIRQ(m_irqn_type);
	//NVIC_EnableIRQ(PININT_NVIC_NAME);

	return true;
}


bool CGpioInt_LPC4370::Open(const CHIP_GPIO chip_gpio, const uint8_t pin_int, const GPIO_INT_TYPE gpio_int_type)
{

	if (CGpio_LPC4370::Open(chip_gpio, GPIO_DIR_INPUT, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLUP)) == false)
		return false;

	if (update_argument(pin_int) == false)
		return false;

	if (gpio_init_config(gpio_int_type) == false)
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



