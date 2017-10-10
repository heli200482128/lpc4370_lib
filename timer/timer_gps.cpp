#include "timer_gps.h"



CTimer_gps::CTimer_gps()
{
}


CTimer_gps::~CTimer_gps()
{
}

bool CTimer_gps::IRQHandle()
{
	if (CTimer_LPC4370::IRQHandle() == false)	return false;

	//todo

	return true;
}