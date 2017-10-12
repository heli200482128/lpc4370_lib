#ifndef __GPIO_LPC4370_H
#define __GPIO_LPC4370_H

#include "chip.h"
#include "common\common.h"

#define INVALID_PORT_PIN	(0xFF)
#define INVALID_PIN_INT		(0xFF)

class CGpio_LPC4370
{
public:
	CGpio_LPC4370();
	~CGpio_LPC4370(){}

	bool Open(const uint8_t &chip_port, const uint8_t &chip_pin, 
				const uint8_t &gpio_port, const uint8_t &gpio_pin, const uint16_t &config);
	virtual void Close();

	static void Init() {	Chip_GPIO_Init(LPC_GPIO_PORT);	}

protected:
	uint8_t m_chip_port;
	uint8_t m_chip_pin;
	uint8_t m_gpio_port;
	uint8_t m_gpio_pin;

	bool argument_valid();
	

};

class CGpioInt_LPC4370:public CGpio_LPC4370
{
public:
	CGpioInt_LPC4370() :CGpio_LPC4370(), m_pin_int(INVALID_PIN_INT), m_irqn_type(INVALID_IRQN){}
	~CGpioInt_LPC4370(){}

	bool Open(const uint8_t &chip_port, const uint8_t &chip_pin, 
				const uint8_t &gpio_port, const uint8_t &gpio_pin, 
				const uint16_t &func_index, const uint8_t &pin_int);
	virtual void Close();

	bool Start();
	void Stop();

	void irq_handle() {	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(m_pin_int));	}

	static void Init() { CGpio_LPC4370::Init(); Chip_PININT_Init(LPC_GPIO_PIN_INT); }

private:
	uint16_t	m_pin_int;
	IRQn_Type	m_irqn_type;

	bool update_argument(const uint8_t &pin_int);

	bool gpio_init_config();
};


#endif

