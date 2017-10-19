#include <stdlib.h>
#include "protocol_gps.h"

CProtocolGps::CProtocolGps()
{
	nmea_parser_init(&m_parser);

}

CProtocolGps::~CProtocolGps(void)
{
	nmea_parser_destroy(&m_parser);

}


LIST_GPS_DATA CProtocolGps::split(char *p_raw_data, unsigned int &raw_data_count) 
{
	LIST_GPS_DATA p_list_gps_data = NULL;
	PNODE_GPS_DATA p_node_gps_data_last = NULL;

	char *p_raw_data_unused = p_raw_data;
	char *p_raw_data_found = NULL;
	unsigned int gps_data_length = 0;
	
	while(1)
	{
		p_raw_data_found = strstr(p_raw_data_unused, "\r\n");

		if (p_raw_data_found == NULL)	break;
		
		gps_data_length = p_raw_data_found + 2 - p_raw_data_unused;
		
		PNODE_GPS_DATA p_node_gps_data_tmp = (PNODE_GPS_DATA)malloc(sizeof(NODE_GPS_DATA));
		p_node_gps_data_tmp->next = NULL;
		p_node_gps_data_tmp->p_nmea = (char*)malloc(gps_data_length + 1);
		memmove(p_node_gps_data_tmp->p_nmea, p_raw_data_unused, gps_data_length);
		p_node_gps_data_tmp->p_nmea[gps_data_length] = '\0';
		
		convertTypeToGP(p_node_gps_data_tmp->p_nmea);

		if (p_list_gps_data == NULL)
		{
			p_list_gps_data = p_node_gps_data_last = p_node_gps_data_tmp;
		}
		else
		{
			p_node_gps_data_last->next = p_node_gps_data_tmp;
			p_node_gps_data_last = p_node_gps_data_tmp;
		}

		p_raw_data_unused = p_raw_data_found + 2;
	}

	raw_data_count = p_raw_data_unused - p_raw_data;

	return p_list_gps_data;
}

/********************************************************************************************/
/* Function: split raw gps string into list of gps data                                     */
/* Argument: p_raw_data    [I] - raw gps string, ends with '\0'                             */
/*           raw_data_count[I] - size of raw gps string									    */
/*           list_gps_data [O] - list of gps data (nmea datagram -- $...\r\n\0),            */
/*                               be null when comes in 			                            */
/* Return  : >=0 - size of used raw gps string                                              */
/*           -1  - error occured                                                            */
/********************************************************************************************/

int CProtocolGps::split(const char *p_raw_data, const int raw_data_count, LIST_GPS_DATA &list_gps_data)
{
	if (list_gps_data)	return -1;

	PNODE_GPS_DATA p_node_gps_data_last = NULL;

	char *p_raw_data_unused = (char*)p_raw_data;
	char *p_raw_data_found = NULL;
	unsigned int gps_data_length = 0;
	
	while(1)
	{
		p_raw_data_found = strstr(p_raw_data_unused, "\r\n");

		if (p_raw_data_found == NULL)	break;
		
		gps_data_length = p_raw_data_found + 2 - p_raw_data_unused;
		
		PNODE_GPS_DATA p_node_gps_data_tmp = (PNODE_GPS_DATA)malloc(sizeof(NODE_GPS_DATA));
		p_node_gps_data_tmp->next = NULL;
		p_node_gps_data_tmp->p_nmea = (char*)malloc(gps_data_length + 1);
		memmove(p_node_gps_data_tmp->p_nmea, p_raw_data_unused, gps_data_length);
		p_node_gps_data_tmp->p_nmea[gps_data_length] = '\0';
		
//		convertTypeToGP(p_node_gps_data_tmp->p_nmea);

		if (list_gps_data == NULL)
		{
			list_gps_data = p_node_gps_data_last = p_node_gps_data_tmp;
		}
		else
		{
			p_node_gps_data_last->next = p_node_gps_data_tmp;
			p_node_gps_data_last = p_node_gps_data_tmp;
		}

		p_raw_data_unused = p_raw_data_found + 2;
	}

	return p_raw_data_unused - p_raw_data;

}
	

void CProtocolGps::parse(const char *p_gps_data, nmeaINFO &gps_info)
{
	nmeaINFO gps_info_tmp;

	nmea_zero_INFO(&gps_info_tmp);

	nmea_parse(&m_parser, p_gps_data, strlen(p_gps_data), &gps_info_tmp);
	
	memcpy(&gps_info, &gps_info_tmp, sizeof(nmeaINFO));

}



void CProtocolGps::convertTypeToGP(char *p_gps_data)
{
	if (p_gps_data == NULL)	return;

	GPS_TYPE gps_type = GPS_GP;

	char *p_gps_start = p_gps_data;

	//check source gps type
	if ((p_gps_start = strstr(p_gps_data, HEADER_GPS_GP)) != NULL)	return;
	else if ((p_gps_start = strstr(p_gps_data, HEADER_GPS_GL)) != NULL)	gps_type = GPS_GL;
	else if ((p_gps_start = strstr(p_gps_data, HEADER_GPS_BD)) != NULL)	gps_type = GPS_BD;
	else if ((p_gps_start = strstr(p_gps_data, HEADER_GPS_GN)) != NULL)	gps_type = GPS_GN;
	else return;


	//replace source gps header to gps_gp header
	switch (gps_type)
	{
	case GPS_GP:	return;
	case GPS_BD:	memcpy(p_gps_start, HEADER_GPS_BD, strlen(HEADER_GPS_BD));	break;
	case GPS_GL:	memcpy(p_gps_start, HEADER_GPS_GL, strlen(HEADER_GPS_GL));	break;
	case GPS_GN:	memcpy(p_gps_start, HEADER_GPS_GN, strlen(HEADER_GPS_GN));	break;
	default:		return;
	}

	char *p_gps_end = strstr(p_gps_start, "*");
	if (p_gps_end == NULL)	return;


	//calculate crc of gps_gp data
	unsigned char crc = 0;
	for (char *p_char = p_gps_start+1; p_char < p_gps_end; p_char++)
	{
		crc ^= *p_char;
	}

	
	//replace new crc to gps_gp crc
	sprintf(p_gps_end + 1, "%02x", crc);

}





