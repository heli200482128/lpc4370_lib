#ifndef __TIMER_MANCHESTER
#define __TIMER_MANCHESTER

#include "timer_lpc4370.h"
class CTimer_manchester :
	public CTimer_LPC4370
{
public:
	CTimer_manchester();
	~CTimer_manchester();

	bool irq_handle();
};


#endif

