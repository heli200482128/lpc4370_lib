#include "timer_manchester.h"



CTimer_manchester::CTimer_manchester()
{
}


CTimer_manchester::~CTimer_manchester()
{
}

bool CTimer_manchester::IRQHandle()
{
	if (CTimer_LPC4370::IRQHandle() == false)	return false;


	return true;
}
