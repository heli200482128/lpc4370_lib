#ifndef __TIMER_GPS
#define __TIMER_GPS


#include "timer_lpc4370.h"
class CTimer_gps :	public CTimer_LPC4370
{
public:
	CTimer_gps();
	~CTimer_gps();

	bool IRQHandle();
};



#endif

