#ifndef __LPC4370_TIMER
#define __LPC4370_TIMER

#include "chip.h"

typedef enum 
{
	LPC4370_TIMER_UNKNOWN = -1,
	LPC4370_TIMER_0 = 0,
	LPC4370_TIMER_1,
	LPC4370_TIMER_2,
	LPC4370_TIMER_3,
}LPC4370_TIMER_NO;

typedef enum
{
	LPC4370_TIMER_OK = 0,
	LPC4370_TIMER_ERR_NO,
	LPC4370_TIMER_ERR_FREQ,
	LPC4370_TIMER_ERR_FUNC,
	LPC4370_TIMER_ERR_ARG,
}LPC4370_TIMER_RESULT;

typedef struct __lpc4370_timer_info
{
	LPC_TIMER_T*	pReg;
	CHIP_RGU_RST_T	rgu_reset;
	CHIP_CCU_CLK_T	ccu_clk;
	IRQn_Type		irq_type;
}LPC4370_TIMER_INFO, *PLPC4370_TIMER_INFO;

typedef void (*func_timer_action)();

typedef struct __lpc4370_timer 
{
	LPC4370_TIMER_NO no;
	PLPC4370_TIMER_INFO pInfo;

	unsigned int frequency;	//Hz
	func_timer_action pFuncAction;

}LPC4370_TIMER, *PLPC4370_TIMER;


#ifdef _cplusplus
extern "C"{
#endif

LPC4370_TIMER_RESULT LPC4370Timer_init(PLPC4370_TIMER pTimer, LPC4370_TIMER_NO timer_no);
LPC4370_TIMER_RESULT LPC4370Timer_setup(PLPC4370_TIMER pTimer, unsigned int timer_freq, func_timer_action func_action);
LPC4370_TIMER_RESULT LPC4370Timer_start(PLPC4370_TIMER pTimer);
LPC4370_TIMER_RESULT LPC4370Timer_stop(PLPC4370_TIMER pTimer);

LPC4370_TIMER_RESULT LPC4370Timer_funcIRQ(PLPC4370_TIMER pTimer);

#ifdef _cplusplus
}
#endif


#endif
