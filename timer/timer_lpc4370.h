#ifndef __TIMER_LPC4370
#define __TIMER_LPC4370

#include "chip.h"
#include "common\common.h"

class CTimer_LPC4370
{
public:
	CTimer_LPC4370();
	~CTimer_LPC4370();

	bool Open(LPC_TIMER_T* pReg, uint32_t frequency, uint32_t priority = 0);
	void Close();

	bool Start();
	void Stop();

	virtual bool irq_handle();

	bool isValid() { return REG_IS_VALID(m_pReg_timer); }
	bool isRunning() { return m_bRunning; }

public:
	LPC_TIMER_T*	m_pReg_timer;
	int8_t			m_matchnum;

protected:

	bool	m_bRunning;

private:
	uint32_t		m_frequency;

	CHIP_RGU_RST_T	m_rgu_reset;
	CHIP_CCU_CLK_T	m_ccu_clk;
	IRQn_Type		m_irqn_type;

	bool update_argument(LPC_TIMER_T* pReg, uint32_t frequency);

};



#endif

