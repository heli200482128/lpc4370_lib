#ifndef __SPI_SGPIO_H
#define __SPI_SGPIO_H

#include "sgpio_lpc4370.h"


/* clock polarity can be either high or low when idle */
typedef enum clockPolarity_t { IDLE_LOW = 0, IDLE_HIGH = 1 } clockPolarity_t;

/* clock phase */
/* CPHA = 0 means the data is sampled on the first edge, shifted on the second edge */
/* CPHA = 1 means the data is shifted on the first edge, sampled on the second edge */
typedef enum clockPhase_t { CPHA_0 = 0, CPHA_1 = 1 } clockPhase_t;

/* supported transmission sizes for the data */
typedef enum wordSize_t {
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

/* peripheral mode */
typedef enum mode_t { MASTER, SLAVE } mode_t;

/* transmission bit rate in Hz (SPI clock speed) */
typedef uint32_t bitRate_t;


typedef enum CsPatternIdx {
	CS_DBIT_4 = 0,
	CS_DBIT_8,
	CS_DBIT_10,
	CS_DBIT_12,
	CS_DBIT_14,
	CS_DBIT_16,
	CS_DBIT_18,
	CS_DBIT_20,
	CS_DBIT_22,
	CS_DBIT_24,
	CS_DBIT_26,
	CS_DBIT_28,
	CS_DBIT_30,
	CS_DBIT_32,
	NUM_CS_IDX,

} CsPatternEntry;

#define CHIP_SELECT_PATTERN(n) CS_ ## n

typedef uint32_t CsPattern;


class CSpiMaster_SGpio
{
typedef struct __spi_signal
{
	CSGpio_Pin_LPC4370 pin;
	CSGpio_Slice_LPC4370 slice;
}SPI_SIGNAL;

public:
	CSpiMaster_SGpio();
	~CSpiMaster_SGpio();

	bool Open(const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_cs, 
				const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_clk, 
				const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_mosi, 
				const CSGpio_Pin_LPC4370::CHIP_SGPIO chip_sgpio_miso);

	void Close();
private:
	SPI_SIGNAL m_cs;
	SPI_SIGNAL m_clk;
	SPI_SIGNAL m_mosi;
	SPI_SIGNAL m_miso;
};


#endif


