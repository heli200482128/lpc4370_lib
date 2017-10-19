#include "timer_lpc4370.h"
#include "common\common.h"


CTimer_LPC4370::CTimer_LPC4370():m_pReg_timer(INVALID_REG)
{
}


CTimer_LPC4370::~CTimer_LPC4370()
{
}

bool CTimer_LPC4370::Open(LPC_TIMER_T* pReg, uint32_t frequency)
{
	if (REG_IS_VALID(m_pReg_timer))	Close();

	if (update_argument(pReg, frequency) == false)	return false;


	/* Get timer peripheral clock rate */
	uint32_t timerFreq = Chip_Clock_GetRate(m_ccu_clk);

	/* Enable timer clock and reset it */
	Chip_TIMER_Init(m_pReg_timer);
	Chip_RGU_TriggerReset(m_rgu_reset);
	while (Chip_RGU_InReset(m_rgu_reset)) {}


	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(m_pReg_timer);
	Chip_TIMER_MatchEnableInt(m_pReg_timer, LPC4370_TIMER_INDEX(m_pReg_timer));
	Chip_TIMER_SetMatch(m_pReg_timer, LPC4370_TIMER_INDEX(m_pReg_timer),
		(Chip_Clock_GetRate(m_ccu_clk) / m_frequency) - 1);
	Chip_TIMER_ResetOnMatchEnable(m_pReg_timer, LPC4370_TIMER_INDEX(m_pReg_timer));
	Chip_TIMER_Enable(m_pReg_timer);


	return true;
}
void CTimer_LPC4370::Close()
{
	if (!REG_IS_VALID(m_pReg_timer))	return;

	Stop();

	/* Disable timer */
	Chip_TIMER_Disable(m_pReg_timer);

	m_pReg_timer = INVALID_REG;
}

bool CTimer_LPC4370::Start()
{
	if (!REG_IS_VALID(m_pReg_timer))	return false;

	/* Enable timer interrupt */
	NVIC_EnableIRQ(m_irqn_type);
	NVIC_ClearPendingIRQ(m_irqn_type);

	m_bRunning = true;

	return true;
}

void CTimer_LPC4370::Stop()
{
	if (!REG_IS_VALID(m_pReg_timer))	return;

	/* Disable timer interrupt */
	NVIC_DisableIRQ(m_irqn_type);

	m_bRunning = false;


}

inline bool CTimer_LPC4370::irq_handle()
{
	// this function is called in IRQHandle, so do not check if m_pReg_timer is valid to save time

	if (Chip_TIMER_MatchPending(m_pReg_timer, LPC4370_TIMER_INDEX(m_pReg_timer)) == false)	return false;

	Chip_TIMER_ClearMatch(m_pReg_timer, LPC4370_TIMER_INDEX(m_pReg_timer));

	return true;
}


inline bool CTimer_LPC4370::update_argument(LPC_TIMER_T* pReg, uint32_t frequency)
{
	if (REG_IS_VALID(m_pReg_timer))	return false;
	if (frequency == 0)	return false;

	switch ((int)pReg)
	{
	case (int)LPC_TIMER0:	m_rgu_reset = RGU_TIMER0_RST;	m_ccu_clk = CLK_MX_TIMER0;	m_irqn_type = TIMER0_IRQn;	break;
	case (int)LPC_TIMER1:	m_rgu_reset = RGU_TIMER1_RST;	m_ccu_clk = CLK_MX_TIMER1;	m_irqn_type = TIMER1_IRQn;	break;
	case (int)LPC_TIMER2:	m_rgu_reset = RGU_TIMER2_RST;	m_ccu_clk = CLK_MX_TIMER2;	m_irqn_type = TIMER2_IRQn;	break;
	case (int)LPC_TIMER3:	m_rgu_reset = RGU_TIMER3_RST;	m_ccu_clk = CLK_MX_TIMER3;	m_irqn_type = TIMER3_IRQn;	break;
	default:	return false;
	}

	m_pReg_timer = pReg;
	m_frequency = frequency;

	return true;
}



