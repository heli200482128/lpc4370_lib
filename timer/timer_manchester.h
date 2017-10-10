#ifndef __TIMER_LPC4370
#define __TIMER_LPC4370

#include "timer_lpc4370.h"
class CTimer_manchester :
	public CTimer_LPC4370
{
public:
	CTimer_manchester();
	~CTimer_manchester();

	bool IRQHandle();
};


#endif

