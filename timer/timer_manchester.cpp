#include "timer_manchester.h"



CTimer_manchester::CTimer_manchester()
{
}


CTimer_manchester::~CTimer_manchester()
{
}

irq_handlebool CTimer_manchester::IRQHandle()
{
	if (CTimer_LPC4370::irq_handle() == false)	return false;


	return true;
}
