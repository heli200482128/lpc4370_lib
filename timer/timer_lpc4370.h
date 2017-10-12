#ifndef __TIMER_LPC4370
#define __TIMER_LPC4370

#include "chip.h"

#define LPC4370_TIMER_INDEX(pReg_timer) (pReg_timer - LPC_TIMER0)

class CTimer_LPC4370
{
public:
	CTimer_LPC4370();
	~CTimer_LPC4370();

	bool Open(LPC_TIMER_T* pReg, uint32_t frequency);
	void Close();

	bool Start();
	void Stop();

	virtual bool irq_handle();

private:
	LPC_TIMER_T* m_pReg_timer;
	uint32_t m_frequency;

	CHIP_RGU_RST_T	m_rgu_reset;
	CHIP_CCU_CLK_T	m_ccu_clk;
	IRQn_Type		m_irqn_type;

	bool update_argument(LPC_TIMER_T* pReg, uint32_t frequency);

};



#endif

