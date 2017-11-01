#include "common.h"



void delay_us(unsigned int t_us)
{
	unsigned int count_1us;
	for (unsigned int i = 0; i < t_us; i++)
	{
		//one iteration takes 1us,
		//!!!adjust count_1us manually if board clock changed
		count_1us = 49;
		while (count_1us--);
	}

}

