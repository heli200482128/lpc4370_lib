#ifndef __UART_INT_LPC4370
#define __UART_INT_LPC4370

#include "chip.h"


class CUartInt_LPC4370
{
#define UART_BUFFER_SIZE	256

public:

	CUartInt_LPC4370();
	~CUartInt_LPC4370();

	bool Open(const LPC_USART_T *pReg, const uint32_t &baudrate, const uint32_t &config);
	void Close();

	bool Start();
	void Stop();

	virtual void irq_handle() { Chip_UART_IRQRBHandler(m_pReg_uart, &m_rxring, &m_txring); }

	int get_rx_count() { return RingBuffer_GetCount(&m_rxring); }

	int Read(uint8_t *pRx, int rx_count_max);
	int Write(uint8_t *pTx, int tx_count);

private:
	LPC_USART_T *m_pReg_uart;
	IRQn_Type	m_irqn_type;

	uint8_t	m_rxbuff[UART_BUFFER_SIZE];
	uint8_t	m_txbuff[UART_BUFFER_SIZE];

public:
	RINGBUFF_T	m_txring;
	RINGBUFF_T	m_rxring;

private:
	
	bool update_argument(const LPC_USART_T *pReg);

	bool uart_init();	
	
	bool uart_config(const uint32_t &baudrate, const uint32_t &config);
};


#endif

