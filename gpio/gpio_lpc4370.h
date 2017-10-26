#ifndef __GPIO_LPC4370_H
#define __GPIO_LPC4370_H

#include "chip.h"
#include "common\common.h"

#define INVALID_PORT_PIN	(0xFF)
#define INVALID_PIN_INT		(0xFF)

#define GPIO_DIR_OUTPUT	(true)
#define GPIO_DIR_INPUT	(false)

class CGpio_LPC4370
{
//type definition
public:
	typedef struct __chip_gpio
	{
		uint8_t chip_port;
		uint8_t chip_pin;
		uint8_t gpio_port;
		uint8_t gpio_pin;
		uint16_t func_index;
	}CHIP_GPIO;

//member functions and variables
public:
	CGpio_LPC4370();
	~CGpio_LPC4370(){}

	//bool Open(const uint8_t chip_port, const uint8_t chip_pin, 
	//			const uint8_t gpio_port, const uint8_t gpio_pin, 
	//			const bool output, const uint16_t config);

	bool Open(const CHIP_GPIO chip_gpio, const bool output, const uint16_t config);

	virtual void Close();

	void Write(bool On) { Chip_GPIO_SetPinState(LPC_GPIO_PORT, m_gpio_port, m_gpio_pin, On); }

protected:
	//uint8_t m_chip_port;
	//uint8_t m_chip_pin;
	uint8_t m_gpio_port;
	uint8_t m_gpio_pin;


	bool gpio_valid();
	//bool argument_valid(CHIP_GPIO chip_gpio);


};


class CGpioInt_LPC4370:public CGpio_LPC4370
{
public:
	
	typedef enum __gpio_int_type
	{
		GPIO_INT_EDGE_RISING = 0,
		GPIO_INT_EDGE_FALLING,
		GPIO_INT_LEVEL_HIGH,
		GPIO_INT_LEVEL_LOW,
	}GPIO_INT_TYPE;
	
	CGpioInt_LPC4370() :CGpio_LPC4370(), m_pin_int(INVALID_PIN_INT), m_irqn_type(INVALID_IRQN){}
	~CGpioInt_LPC4370(){}

//	bool Open(const uint8_t chip_port, const uint8_t chip_pin, 
//				const uint8_t gpio_port, const uint8_t gpio_pin, const uint16_t func_index, 
//				const uint8_t pin_int, const GPIO_INT_TYPE gpio_int_type);

	bool Open(const CHIP_GPIO chip_gpio, const uint8_t pin_int, const GPIO_INT_TYPE gpio_int_type);


	virtual void Close();

	bool Start();
	void Stop();

	void irq_handle() {	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));	}

private:
	uint16_t	m_pin_int;
	IRQn_Type	m_irqn_type;

	bool update_argument(const uint8_t &pin_int);

	bool gpio_init_config(const GPIO_INT_TYPE gpio_int_type);
};


#endif

