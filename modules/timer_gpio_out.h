#ifndef __TIMER_GPIO_OUT_H
#define __TIMER_GPIO_OUT_H

#include <stdlib.h>
#include <string.h>
#include "timer\timer_lpc4370.h"
#include "gpio\gpio_lpc4370.h"

typedef struct __gpio_sequence
{
	bool			*p_data;
	unsigned int	data_length;
	unsigned int	data_offset_unused;
	void reinit()
	{	
		if(p_data)	free(p_data);
		p_data = NULL;
		data_length = 0;
		data_offset_unused = 0;
	}
	
	void setup(bool *p_data_new, unsigned int data_length_new)
	{
		if(p_data_new == NULL || data_length_new == 0)	return;
		p_data = (bool *)malloc(data_length_new * sizeof(bool));
		memmove(p_data, p_data_new, data_length_new * sizeof(bool));
		data_length = data_length_new;
		data_offset_unused = 0;
	}
}GPIO_SEQUENCE;

class CTimer_GpioOut : public CTimer_LPC4370, public CGpio_LPC4370
{
public:
	CTimer_GpioOut();
	~CTimer_GpioOut();

//	bool Init(LPC_TIMER_T* pRegTimer, uint32_t frequency,
//				const uint8_t &chip_port, const uint8_t &chip_pin,
//				const uint8_t &gpio_port, const uint8_t &gpio_pin, const uint16_t &config);

	bool Init(LPC_TIMER_T* pRegTimer, uint32_t frequency,
				const CHIP_GPIO chip_gpio, const uint16_t &config);
	void Deinit();

	bool Start(bool *p_sequence, unsigned int sequence_length);
	virtual void Stop();

	virtual bool irq_handle();

	bool transfer_over() { return m_gpio_sequence.data_length == m_gpio_sequence.data_offset_unused; }

private:
	GPIO_SEQUENCE	m_gpio_sequence;



};


#endif

