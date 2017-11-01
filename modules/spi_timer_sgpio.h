#ifndef __SPI_TIMER_SGPIO_H
#define __SPI_TIMER_SGPIO_H

#include <string.h>
#include "timer\timer_lpc4370.h"
#include "sgpio\sgpio_lpc4370.h"


/* clock polarity can be either high or low when idle */
typedef enum clockPolarity_t { CPOL_0 = 0, CPOL_1 = 1 } clockPolarity_t;

/* clock phase */
/* CPHA = 0 means the data is sampled on the first edge, shifted on the second edge */
/* CPHA = 1 means the data is shifted on the first edge, sampled on the second edge */
typedef enum clockPhase_t { CPHA_0 = 0, CPHA_1 = 1 } clockPhase_t;

/* supported transmission sizes for the data */
typedef enum wordSize_t {
	DBIT_INVALID = 0,
	DBIT_4 = 4,
	DBIT_8 = 8,
	DBIT_10 = 10,
	DBIT_12 = 12,
	DBIT_14 = 14,
	DBIT_16 = 16,
	DBIT_18 = 18,
	DBIT_20 = 20,
	DBIT_22 = 22,
	DBIT_24 = 24,
	DBIT_26 = 26,
	DBIT_28 = 28,
	DBIT_30 = 30,
	DBIT_32 = 32,
} wordSize_t;

/* transmission bit rate in Hz (SPI clock speed) */
typedef uint32_t bitRate_t;


typedef struct __spi_outreg
{
#define SPI_HEAD_LENGTH	50	//waiting for timer to be stable

	uint32_t reg[SPI_HEAD_LENGTH + 130];	//value = clk_value<<clk_offset | mosi_value<<clk_offset
											//length = SPI_HEAD_LENGTH+spi_bitcount_max*2+2
	int offset;	//sent sequence count
	int length;	//sequence valid length
	void reinit()
	{
		memset(reg, 0, (SPI_HEAD_LENGTH + 130) * sizeof(uint32_t));
		offset = 0;
		length = 0;
	}
}SPI_OUTREG;

class CSpiMaster_Timer_SGpio : public CTimer_LPC4370
{
#define SPI_DEV_VALID()	(CTimer_LPC4370::isValid() && m_sgpio_pin_cs.isValid() && \
							m_sgpio_pin_clk.isValid() && m_sgpio_pin_mosi.isValid())
#define SPI_PARAM_VALID() (m_wordLenght != DBIT_INVALID)
public:
	CSpiMaster_Timer_SGpio();
	~CSpiMaster_Timer_SGpio();

	bool Open(LPC_TIMER_T* pRegTimer, uint32_t frequency, 
				CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_cs,
				CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_clk,
				CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_mosi);

	bool Setup(wordSize_t wordLenght, clockPhase_t clockPhase, 
				clockPolarity_t	clockPolarity, bitRate_t bitRateHz);

	void Close();

	bool Start(uint32_t data);
	void Stop();

	virtual bool irq_handle();

public:

	SPI_OUTREG	m_spi_outreg;

	uint32_t m_spi_outreg_mask;

private:
	//hw signals
	CSGpio_Pin_LPC4370 m_sgpio_pin_cs;
	CSGpio_Pin_LPC4370 m_sgpio_pin_clk;
	CSGpio_Pin_LPC4370 m_sgpio_pin_mosi;


	//spi parameters
	wordSize_t		m_wordLenght;			// select the number of bits transmitted per word
	clockPhase_t 	m_clockPhase;			// SPI phase 0 or 1 transfer format
	bitRate_t		m_bitRateHz;			// speed of the spi interface

	//temporary
	bool		m_clk_idle_level;

	void reset_signals() {
		m_sgpio_pin_cs.Set_GpioOutReg(true);
		m_sgpio_pin_mosi.Set_GpioOutReg(false);
		m_sgpio_pin_clk.Set_GpioOutReg(m_clk_idle_level);
	}
};


#endif

