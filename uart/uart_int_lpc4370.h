#ifndef __UART_INT_LPC4370
#define __UART_INT_LPC4370

#include "chip.h"


class CUartInt_LPC4370
{
#define BUFFER_SIZE	256

public:

	CUartInt_LPC4370();
	~CUartInt_LPC4370();

	bool open(LPC_USART_T *hUart, uint32_t baudrate, uint32_t config);
	void close();

	bool start();
	void stop();

	void IRQHandle() { Chip_UART_IRQRBHandler(m_pReg_uart, &m_rxring, &m_txring); }

	int getRxCount() { return RingBuffer_GetCount(&m_rxring); }

	int Read(uint8_t *pRx, int rx_count_max);
	int Write(uint8_t *pTx, int tx_count);

private:
	LPC_USART_T *m_pReg_uart;
	IRQn_Type	m_irqn_type;

	uint8_t	m_rxbuff[BUFFER_SIZE];
	uint8_t	m_txbuff[BUFFER_SIZE];

public:
	RINGBUFF_T	m_txring;
	RINGBUFF_T	m_rxring;

private:
	//bool handle_valid(LPC_USART_T *hUart) {
	//	return hUart == LPC_USART0 || hUart == LPC_USART2 || hUart == LPC_USART3;
	//}
	
	bool update_argument(LPC_USART_T *pReg);
};


#endif

