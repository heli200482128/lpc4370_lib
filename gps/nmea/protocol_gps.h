#ifndef __PROTOCOL_GPS
#define __PROTOCOL_GPS

#include <vector>
#include <string>
#include "nmea/parser.h"
using namespace std;

#define HEADER_GPS_GP	("$GP")
#define HEADER_GPS_GL	("$GL")
#define HEADER_GPS_BD	("$BD")
#define HEADER_GPS_GN	("$GN")

typedef enum __gps_type
{
	GPS_GP = 0,
	GPS_GL,
	GPS_BD,
	GPS_GN,
}GPS_TYPE;

typedef struct __node_gps_data
{
	char					*p_nmea;			//pointer of complete nmea1083, ends with '\0'
	struct __node_gps_data	*next;	
}NODE_GPS_DATA, *PNODE_GPS_DATA, *LIST_GPS_DATA;
//
//typedef struct __raw_gps_data
//{
//	char *	p_data;
//	unsigned int data_count;	//valid data number
//	unsigned int data_size;		//max data number
//}RAW_GPS_DATA;

class CProtocolGps
{
public:
	CProtocolGps();
	~CProtocolGps(void);



	LIST_GPS_DATA split(char *raw_data, unsigned int &raw_data_count);	//split raw_data into list of gps_data (nmea datagram -- $...\r\n\0),
																																			//raw_data needs to end with '\0'
																																			//and store used length of raw_data in raw_data_count
	int split(const char *p_raw_data, const int raw_data_count, LIST_GPS_DATA &list_gps_data);
																																			
																																			
	void parse(const char *p_gps_data, nmeaINFO &gps_info);


private:

	nmeaPARSER m_parser;


	void convertTypeToGP(char *p_gps_data);	// 'p_gps_data' needs to be a complete and single nmea massage and ends with '\0'
											// convert 'p_gps_data' from different starts, like $GP/$BD/$GP/$GN to unique start $GP
											// store the result back to 'p_gps_data'


};




#endif // !__PROTOCOL_GPS


