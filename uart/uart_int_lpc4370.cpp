#include "uart_int_lpc4370.h"



CUartInt_LPC4370::CUartInt_LPC4370()
	:m_pReg_uart(0), m_irqn_type(RESERVED2_IRQn)
{
}


CUartInt_LPC4370::~CUartInt_LPC4370()
{
}


bool CUartInt_LPC4370::update_argument(LPC_USART_T *pReg)
{
	if (m_pReg_uart)	return false;

	switch ((int)pReg)
	{
	case (int)LPC_USART0:	m_irqn_type = USART0_IRQn;	break;
	case (int)LPC_USART2:	m_irqn_type = USART2_IRQn;	break;
	case (int)LPC_USART3:	m_irqn_type = USART3_IRQn;	break;
	default:			return false;
	}

	m_pReg_uart = pReg;
	return true;
}

bool CUartInt_LPC4370::open(LPC_USART_T *pReg, uint32_t baudrate, uint32_t config)
{
static uint8_t uartABComplete[] = "UART Auto-Baudrate synchronized! \n\r";

	if (m_pReg_uart != 0)	close();

	if (update_argument(pReg) == false)	return false;
	
	Chip_UART_Init(m_pReg_uart);
	Chip_UART_SetBaud(m_pReg_uart, baudrate);
	Chip_UART_ConfigData(m_pReg_uart, config); /* Default 8-N-1 */

	/* Enable UART Transmit */
	Chip_UART_TXEnable(m_pReg_uart);

	/* Reset FIFOs, Enable FIFOs and DMA mode in UART */
	Chip_UART_SetupFIFOS(m_pReg_uart, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
		UART_FCR_TX_RS | UART_FCR_DMAMODE_SEL | UART_FCR_TRG_LEV0));

	/* Enable UART End of Auto baudrate & Auto baudrate timeout interrupts */
	Chip_UART_IntEnable(m_pReg_uart, (UART_IER_ABEOINT | UART_IER_ABTOINT));

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(m_irqn_type, 1);
	/* Enable Interrupt for UART0 channel */
	NVIC_EnableIRQ(m_irqn_type);

//	/* ---------------------- Auto baud rate section ----------------------- */
//	/* Start auto baudrate mode */
//	Chip_UART_ABCmd(m_pReg_uart, UART_ACR_MODE0, true, ENABLE);

//	/* Loop until auto baudrate mode complete */
//	while (Chip_UART_GetABEOStatus(m_pReg_uart) == RESET) {}

	/* Send UART Autobaud completed message */
	Chip_UART_SendBlocking(m_pReg_uart, uartABComplete, sizeof(uartABComplete));

		/* Disable UART Interrupt */
	NVIC_DisableIRQ(m_irqn_type);

	/* Enable UART Rx & line status interrupts */
	/*
	* Do not enable transmit interrupt here, since it is handled by
	* UART_Send() function, just to reset Tx Interrupt state for the
	* first time
	*/
	Chip_UART_IntEnable(m_pReg_uart, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* Before using the ring buffers, initialize them using the ring
	buffer init function */
	RingBuffer_Init(&m_rxring, m_rxbuff, 1, BUFFER_SIZE);
	RingBuffer_Init(&m_txring, m_txbuff, 1, BUFFER_SIZE);

	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(m_irqn_type, 1);
	///* Enable Interrupt for UART channel */
	//NVIC_EnableIRQ(m_irqn_type);

	return true;
}

void CUartInt_LPC4370::close()
{
	if (m_pReg_uart == 0)	return;

	stop();

	/* Disable UART Rx & line status interrupts */
	Chip_UART_IntDisable(m_pReg_uart, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* Disable Interrupt for UART channel */
	NVIC_DisableIRQ(m_irqn_type);

	/* Wait for current transmission complete - THR must be empty */
	while (Chip_UART_CheckBusy(m_pReg_uart) == SET) {}

	/* DeInitialize UART0 peripheral */
	Chip_UART_DeInit(m_pReg_uart);

	m_pReg_uart = 0;

	return;
}

bool CUartInt_LPC4370::start()
{
	if (m_pReg_uart == 0)	return false;

	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(m_irqn_type);

	return true;
}
void CUartInt_LPC4370::stop()
{
	if (m_pReg_uart == 0) return;

	/* Disable Interrupt for UART channel */
	NVIC_DisableIRQ(m_irqn_type);

}


int CUartInt_LPC4370::Read(uint8_t *pRx, int rx_count_max)
{
	if (pRx == NULL || rx_count_max == 0)	return 0;

	int rx_count = getRxCount();

	return Chip_UART_ReadRB(m_pReg_uart, &m_rxring, pRx, MIN(rx_count_max, rx_count));

}


int CUartInt_LPC4370::Write(uint8_t *pTx, int tx_count)
{
	if (pTx == NULL || tx_count == 0)	return 0;

	return Chip_UART_SendRB(m_pReg_uart, &m_txring, pTx, tx_count);

}



