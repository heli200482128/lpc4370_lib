#include "Uart_Gps.h"
#include "nmea\sentence.h"


bool CUart_Gps::GetPosition(double &latitude, double  &longitude)
{
	static bool b_result = false;

	//get new raw gps data;
	static char new_raw_gps_data[UART_BUFFER_SIZE];
	static int new_raw_gps_data_length = Read((uint8_t*)new_raw_gps_data, UART_BUFFER_SIZE);

	if (new_raw_gps_data_length == 0)	
		return b_result;

	m_raw_gps_data.append(new_raw_gps_data, new_raw_gps_data_length);

	//check if complete gps data found
	vector<string> list_gps_data;
	split(m_raw_gps_data, list_gps_data);

	if (list_gps_data.empty())	
		return b_result;

	//check if $GPRMC found, if yes, update pos to argument list
	static nmeaINFO gps_info;
	for (vector<string>::iterator it = list_gps_data.begin(); it != list_gps_data.end(); it++)
	{
		parse(*it, gps_info);

		if ((gps_info.smask | GPRMC) == 0) continue;

		if (gps_info.sig)
		{
			b_result = true;
			latitude = gps_info.lat;
			longitude = gps_info.lon;
		}
		else
		{
			b_result = false;
		}
	}

	return b_result;
}
