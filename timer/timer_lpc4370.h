#ifndef __TIMER_LPC4370
#define __TIMER_LPC4370

#include "chip.h"

class CTimer_LPC4370
{
public:
	CTimer_LPC4370();
	~CTimer_LPC4370();

	bool Open(LPC_TIMER_T* pReg, uint32_t frequency);
	void Close();

	bool Start();
	virtual void Stop();

	virtual bool irq_handle();

protected:
	bool	m_bRunning;

private:
	LPC_TIMER_T*	m_pReg_timer;
	uint32_t		m_frequency;

	int8_t			m_matchnum;
	CHIP_RGU_RST_T	m_rgu_reset;
	CHIP_CCU_CLK_T	m_ccu_clk;
	IRQn_Type		m_irqn_type;

	bool update_argument(LPC_TIMER_T* pReg, uint32_t frequency);

};



#endif

