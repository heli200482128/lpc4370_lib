#ifndef __PROTOCOL_GPS
#define __PROTOCOL_GPS

#include <vector>
#include <string>
#include "nmea/parser.h"
using namespace std;

#define HEADER_GPS_GP	string("$GP")
#define HEADER_GPS_BL	string("$BL")
#define HEADER_GPS_BD	string("$BD")
#define HEADER_GPS_GN	string("$GN")

typedef enum __gps_type
{
	GPS_GP = 0,
	GPS_BL,
	GPS_BD,
	GPS_GN,
}GPS_TYPE;

class CProtocolGps
{
public:
	CProtocolGps();
	~CProtocolGps(void);



	void split(string &raw_data, vector<string> &list_gps_data);	//split raw_data into list of gps_data (nmea datagram -- $...\r\n), 
																	//and left incomplete gps_data in raw_data
	void parse(const string &gps_data, nmeaINFO &gps_info);


private:

	nmeaPARSER m_parser;

	string convertTypeToGP(string gps_data);	//'gps_data' needs to be a complete and single gps massage


};




#endif // !__PROTOCOL_GPS


