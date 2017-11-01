#ifndef __TIMER_MANCHESTER_H
#define __TIMER_MANCHESTER_H

#include <stdlib.h>
#include <string.h>
#include "timer\timer_lpc4370.h"
#include "sgpio\sgpio_lpc4370.h"

typedef struct __manchester_outreg
{
#define MANCHESTER_HEAD_LENGTH		4			//waiting for timer to be stable
#define MANCHESTER_DATA_LENGTH_MAX	240	//112*2+16
#define MANCHESTER_END_LENGTH		1			//close switch
	uint32_t	reg[MANCHESTER_HEAD_LENGTH + MANCHESTER_DATA_LENGTH_MAX + MANCHESTER_END_LENGTH];
	int			length;
	int			offset;
	void reinit()
	{
		memset(reg, 0, (MANCHESTER_HEAD_LENGTH + MANCHESTER_DATA_LENGTH_MAX + MANCHESTER_END_LENGTH) * sizeof(uint32_t));
		offset = 0;
		length = 0;
	}

}MANCHESTER_OUTREG;




class CTimer_Manchester : public CTimer_LPC4370
{
public:
	CTimer_Manchester();
	~CTimer_Manchester();

	bool Open(LPC_TIMER_T* pRegTimer, uint32_t frequency,
				const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_V1, 
				const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_V2);
	void Close();

	bool Start(bool *p_sequence, unsigned int sequence_length);
	void Stop();

	//virtual bool irq_handle();

	
public:
	MANCHESTER_OUTREG	m_manchester_outreg;

	uint32_t m_manchester_outreg_mask;


	//int m_sgpio_index_V1;
	//int m_sgpio_index_V2;

private:
	CSGpio_Pin_LPC4370	m_sgpio_V1;
	CSGpio_Pin_LPC4370	m_sgpio_V2;

};


#endif

