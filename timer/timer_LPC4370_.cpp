#include "timer_LPC4370_.h"
#include "board\board.h"

/************   private   ************/
#define LPC4370_TIMER_COUNT	4

#define LPC4370_TIMER_INDEX(lpc4370_timer_no)	((lpc4370_timer_no)-LPC4370_TIMER_0)
#define LPC4370_TIMER_VALID(lpc4370_timer_no)	(((lpc4370_timer_no)-LPC4370_TIMER_0)<LPC4370_TIMER_COUNT)

/*static */LPC4370_TIMER_INFO g_LPC4370_Timer_info[LPC4370_TIMER_COUNT] = {
	{ LPC_TIMER0, RGU_TIMER0_RST, CLK_MX_TIMER0, TIMER0_IRQn },
	{ LPC_TIMER1, RGU_TIMER1_RST, CLK_MX_TIMER1, TIMER1_IRQn },
	{ LPC_TIMER2, RGU_TIMER2_RST, CLK_MX_TIMER2, TIMER2_IRQn },
	{ LPC_TIMER3, RGU_TIMER3_RST, CLK_MX_TIMER3, TIMER3_IRQn },
};



/************   public   ************/

LPC4370_TIMER_RESULT LPC4370Timer_init(PLPC4370_TIMER pTimer, LPC4370_TIMER_NO timer_no)
{
	if (pTimer == NULL)							return LPC4370_TIMER_ERR_ARG;
	if (LPC4370_TIMER_VALID(timer_no) == false)	return LPC4370_TIMER_ERR_NO;
	
	pTimer->no = timer_no;
	pTimer->pInfo = &g_LPC4370_Timer_info[timer_no - LPC4370_TIMER_0];

	pTimer->frequency = 0;
	pTimer->pFuncAction = NULL;

	return LPC4370_TIMER_OK;
}


LPC4370_TIMER_RESULT LPC4370Timer_setup(PLPC4370_TIMER pTimer, unsigned int timer_freq, func_timer_action func_action)
{
	if (pTimer == NULL)								return LPC4370_TIMER_ERR_ARG;
	if (LPC4370_TIMER_VALID(pTimer->no) == false)	return LPC4370_TIMER_ERR_NO;
	if (timer_freq == 0)							return LPC4370_TIMER_ERR_FREQ;
	if (func_action == NULL)						return LPC4370_TIMER_ERR_FUNC;

	pTimer->frequency = timer_freq;
	pTimer->pFuncAction = func_action;

	return LPC4370_TIMER_OK;
}

unsigned int timerFreq;
LPC4370_TIMER_RESULT LPC4370Timer_start(PLPC4370_TIMER pTimer)
{
	if (pTimer == NULL)								return LPC4370_TIMER_ERR_ARG;
	if (LPC4370_TIMER_VALID(pTimer->no) == false)	return LPC4370_TIMER_ERR_NO;
	if (pTimer->frequency == 0)						return LPC4370_TIMER_ERR_FREQ;
	if (pTimer->pFuncAction == NULL)				return LPC4370_TIMER_ERR_FUNC;

	{
	
		PLPC4370_TIMER_INFO pInfo = pTimer->pInfo;

		///* Get timer peripheral clock rate */
		timerFreq = Chip_Clock_GetRate(pInfo->ccu_clk);
		
		/* Enable timer clock and reset it */
		Chip_TIMER_Init(pInfo->pReg);
		Chip_RGU_TriggerReset(pInfo->rgu_reset);
		while (Chip_RGU_InReset(pInfo->rgu_reset)) {}


		/* Timer setup for match and interrupt at TICKRATE_HZ */
		Chip_TIMER_Reset(pInfo->pReg);
		Chip_TIMER_MatchEnableInt(pInfo->pReg, LPC4370_TIMER_INDEX(pTimer->no));
		Chip_TIMER_SetMatch(pInfo->pReg, LPC4370_TIMER_INDEX(pTimer->no), 
							(Chip_Clock_GetRate(pInfo->ccu_clk) / pTimer->frequency)-1);
		Chip_TIMER_ResetOnMatchEnable(pInfo->pReg, LPC4370_TIMER_INDEX(pTimer->no));
		Chip_TIMER_Enable(pInfo->pReg);

		/* Enable timer interrupt */
		NVIC_EnableIRQ(pInfo->irq_type);
		NVIC_ClearPendingIRQ(pInfo->irq_type);

	}

	return LPC4370_TIMER_OK;
}

LPC4370_TIMER_RESULT LPC4370Timer_stop(PLPC4370_TIMER pTimer)
{
	if (pTimer == NULL)								return LPC4370_TIMER_ERR_ARG;
	if (LPC4370_TIMER_VALID(pTimer->no) == false)	return LPC4370_TIMER_ERR_NO;
	
	
	/* Disable timer interrupt */
	NVIC_DisableIRQ(pTimer->pInfo->irq_type);

	/* Disable timer */
	Chip_TIMER_Disable(pTimer->pInfo->pReg);


	return LPC4370_TIMER_OK;
}


LPC4370_TIMER_RESULT LPC4370Timer_funcIRQ(PLPC4370_TIMER pTimer)
{
	if (pTimer == NULL)					return LPC4370_TIMER_ERR_ARG;
	if (pTimer->pFuncAction == NULL)	return LPC4370_TIMER_ERR_FUNC;


	if (Chip_TIMER_MatchPending(pTimer->pInfo->pReg, LPC4370_TIMER_INDEX(pTimer->no))) {
		Chip_TIMER_ClearMatch(pTimer->pInfo->pReg, LPC4370_TIMER_INDEX(pTimer->no));

		pTimer->pFuncAction();
	}

	return LPC4370_TIMER_OK;
}
