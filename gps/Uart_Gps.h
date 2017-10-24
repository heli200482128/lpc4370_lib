#ifndef __UART_GPS_H
#define __UART_GPS_H

#include "uart\uart_int_lpc4370.h"
#include "nmea\protocol_gps.h"



typedef struct __raw_gps_data
{
	char *	p_data;
	unsigned int data_count;	//valid data number
	unsigned int data_size;		//max data number
}RAW_GPS_DATA;


class CUart_Gps : public CUartInt_LPC4370, public CProtocolGps
{
public:
	CUart_Gps();
	~CUart_Gps();

	int GetPosition(double &latitude, double  &longitude, double &altitude);
private:
	nmeaINFO	m_gps_info;

	RAW_GPS_DATA	m_raw_gps_data;	//max count needs to be one less than size 
									//because raw data used to split needs to be end with '\0'

};


#endif

