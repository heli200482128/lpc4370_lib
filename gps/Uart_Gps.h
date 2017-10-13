#ifndef __UART_GPS
#define __UART_GPS

#include <string>
#include "uart\uart_int_lpc4370.h"
#include "nmea\protocol_gps.h"

using namespace std;

class CUart_Gps : public CUartInt_LPC4370, public CProtocolGps
{
public:
	CUart_Gps() :CUartInt_LPC4370(), CProtocolGps() { nmea_zero_INFO(&m_gps_info); }
	~CUart_Gps() {}

	bool GetPosition(double &latitude, double  &longitude);
private:
	nmeaINFO	m_gps_info;

	string	m_raw_gps_data;

};


#endif

