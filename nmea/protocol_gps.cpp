#include <sstream>
#include <iomanip>
#include "protocol_gps.h"
using namespace std;

CProtocolGps::CProtocolGps()
{
	nmea_parser_init(&m_parser);
}

CProtocolGps::~CProtocolGps(void)
{
	nmea_parser_destroy(&m_parser);
}

void CProtocolGps::split(string &raw_data, vector<string> &list_gps_data)
{
	const string raw_data_original = raw_data;

	list_gps_data.clear();

	// split 'gps_data' into single gps messages, and store messages into 'list_gps_data'
	size_t iMsgStart = 0, iMsgEnd = 0;
	while ((iMsgEnd = raw_data_original.find("\r\n", iMsgStart)) != string::npos)
	{
		string gps_data_gp = convertTypeToGP(raw_data_original.substr(iMsgStart, iMsgEnd + 2));

		if (!gps_data_gp.empty())	list_gps_data.push_back(gps_data_gp);

		iMsgStart = iMsgEnd + 2;
	}

}

void CProtocolGps::parse(const string &gps_data, nmeaINFO &gps_info)
{
	nmeaINFO gps_info_tmp;

	nmea_zero_INFO(&gps_info_tmp);

	nmea_parse(&m_parser, gps_data.c_str(), gps_data.size(), &gps_info_tmp);
	
	memcpy(&gps_info, &gps_info_tmp, sizeof(nmeaINFO));

}



string CProtocolGps::convertTypeToGP(string gps_data)
{
	if (gps_data.at(0) != '$' || gps_data.at(gps_data.size()-2) != '\r' || gps_data.at(gps_data.size()-1) != '\n')
	{
		return string();
	}

	GPS_TYPE gps_type = GPS_GP;

	//check source gps type
	if (gps_data.find(HEADER_GPS_GP) != string::npos)		gps_type = GPS_GP;
	else if (gps_data.find(HEADER_GPS_BL) != string::npos)	gps_type = GPS_BL;
	else if (gps_data.find(HEADER_GPS_BD) != string::npos)	gps_type = GPS_BD;
	else if (gps_data.find(HEADER_GPS_GN) != string::npos)	gps_type = GPS_GN;


	//get source gps header
	string src_gps_header;
	switch (gps_type)
	{
	case GPS_GP:return gps_data;
	case GPS_BD:src_gps_header = HEADER_GPS_BD;	break;
	case GPS_BL:src_gps_header = HEADER_GPS_BL;	break;
	case GPS_GN:src_gps_header = HEADER_GPS_GN;	break;
	default:
		return string();
	}

	//replace source gps header to gps_gp header
	string gps_src_data = gps_data.substr(0, gps_data.find('*'));
	string gps_gp_data = gps_src_data.replace(gps_src_data.find(src_gps_header), src_gps_header.size(), HEADER_GPS_GP);

	//calculate crc of gps_gp data
	unsigned char crc = 0;
	for (size_t iData = 1; iData < gps_gp_data.size(); iData++)
	{
		crc ^= gps_gp_data.at(iData);
	}

	//pack gps_gp message
	ostringstream gps_gp;
	gps_gp.clear();
	gps_gp << gps_gp_data << "*" << hex << setw(2) << setfill('0') << (int)crc << dec << "\r\n";

	return gps_gp.str();
}





