#include <stdlib.h>
#include "Uart_Gps.h"
#include "nmea\sentence.h"

CUart_Gps::CUart_Gps() :CUartInt_LPC4370(), CProtocolGps() 
{
	nmea_zero_INFO(&m_gps_info); 

	m_raw_gps_data.p_data = (char*)malloc(UART_BUFFER_SIZE);
	m_raw_gps_data.data_size = UART_BUFFER_SIZE;
	m_raw_gps_data.data_count = 0;
}


CUart_Gps::~CUart_Gps() 
{
	free(m_raw_gps_data.p_data);
}


/********************************************************************************************/
/* Function: get gps position information(altitude/latitude/longitude)                      */
/* Argument: latitude [O] - gps latitude in NDEG - +/-[degree][min].[sec/60](gps format)    */
/*           longitude[O] - gps longitude  in NDEG - +/-[degree][min].[sec/60](gps format)  */
/*           altitude [O] - gps atitude              			                            */
/* Return  :  1 - receive valid gps position information                                    */
/*            0 - no complete nmea package received                                         */
/*           -1 - receive complete nmea package, but gps is unlocated                       */
/********************************************************************************************/

int CUart_Gps::GetPosition(double &latitude, double  &longitude, double &altitude)
{
	int result = 0;

	//get new raw gps data and store into m_raw_gps_data;
	int new_raw_gps_data_length = Read((uint8_t*)(m_raw_gps_data.p_data + m_raw_gps_data.data_count),
										m_raw_gps_data.data_size - 1 - m_raw_gps_data.data_count);
	
	//if no new raw gps data found, return
	if (new_raw_gps_data_length == 0)	
		return result;

	//update length of raw gps data 
	m_raw_gps_data.data_count += new_raw_gps_data_length;

	
	//append '\0' to raw gps data to satisfy the need of split(), but valid count of raw gps data keeps unchanged
	m_raw_gps_data.p_data[m_raw_gps_data.data_count] = '\0';

	//split raw gps data and store complete nmea packages into list_gps_data
	LIST_GPS_DATA list_gps_data = NULL;
	int raw_gps_data_count_used = split(m_raw_gps_data.p_data, m_raw_gps_data.data_count, list_gps_data);

	if (raw_gps_data_count_used <= 0)	return result;
	
	
	//update raw gps data: remove processed data
	m_raw_gps_data.data_count -= raw_gps_data_count_used;
	memmove(m_raw_gps_data.p_data, m_raw_gps_data.p_data + raw_gps_data_count_used, m_raw_gps_data.data_count);
	

	//if no complete gps data found, return
	if (list_gps_data == NULL)	
		return result;

	//if package that starts with $GPRMC found and pos is valid, update pos to argument list
	static nmeaINFO gps_info;
	while (list_gps_data)
	{
		PNODE_GPS_DATA pnode_gps_data = list_gps_data;
		list_gps_data = list_gps_data->next;

		parse(pnode_gps_data->p_nmea, gps_info);
		free(pnode_gps_data->p_nmea);
		free(pnode_gps_data);

		if ((gps_info.smask |GPGGA) == 0) continue;

		if (gps_info.sig)
		{
			result = 1;
			latitude = gps_info.lat;
			longitude = gps_info.lon;
			altitude = gps_info.elv;
		}
		else
		{
			result = -1;
		}

	}


	return result;
}
