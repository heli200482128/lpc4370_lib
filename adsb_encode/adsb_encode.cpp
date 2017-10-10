#include "adsb_encode.h"
#include "lpc_types.h"


/*************   private   ***************************/

#define SIZE_ADSB_POS_DF		5
#define SIZE_ADSB_POS_CA		3
#define SIZE_ADSB_POS_AA		24
#define SIZE_ADSB_POS_ME_TYPE		5
#define SIZE_ADSB_POS_ME_MONITOR	2
#define SIZE_ADSB_POS_ME_ANTENNA	1
#define SIZE_ADSB_POS_ME_ALTITUDE	12
#define SIZE_ADSB_POS_ME_TIME		1
#define SIZE_ADSB_POS_ME_CPR		1
#define SIZE_ADSB_POS_ME_LATITUDE	17
#define SIZE_ADSB_POS_ME_LONGITUDE	17
#define SIZE_ADSB_POS_PI		24


//typedef enum
//{
//	offset_adsb_pos_df			= 0,
//	offset_adsb_pos_ca			= offset_adsb_pos_df			+ SIZE_ADSB_POS_DF,
//	offset_adsb_pos_aa			= offset_adsb_pos_ca			+ SIZE_ADSB_POS_CA,
//	offset_adsb_pos_me_type		= offset_adsb_pos_aa			+ SIZE_ADSB_POS_AA,
//	offset_adsb_pos_me_monitor	= offset_adsb_pos_me_type		+ SIZE_ADSB_POS_ME_TYPE,
//	offset_adsb_pos_me_antenna	= offset_adsb_pos_me_monitor	+ SIZE_ADSB_POS_ME_MONITOR,
//	offset_adsb_pos_me_altitude	= offset_adsb_pos_me_antenna	+ SIZE_ADSB_POS_ME_ANTENNA,
//	offset_adsb_pos_me_time		= offset_adsb_pos_me_altitude	+ SIZE_ADSB_POS_ME_ALTITUDE,
//	offset_adsb_pos_me_cpr		= offset_adsb_pos_me_time		+ SIZE_ADSB_POS_ME_TIME,
//	offset_adsb_pos_me_latitude	= offset_adsb_pos_me_cpr		+ SIZE_ADSB_POS_ME_CPR,
//	offset_adsb_pos_me_longitude= offset_adsb_pos_me_latitude	+ SIZE_ADSB_POS_ME_LATITUDE,
//	offset_adsb_pos_pi			= offset_adsb_pos_me_longitude	+ SIZE_ADSB_POS_ME_LONGITUDE,
//}OFFSET_ADSB_POS;



typedef struct __adsb_data
{
	const unsigned char	df;
	const unsigned char	ca;
	const unsigned int	aa;
	struct  
	{
		const unsigned char		type;
		const unsigned char		monitor;
		const unsigned char		antenna;
		unsigned short			altitude;
		const unsigned char		time;
		unsigned char			cpr;
		unsigned int	latitude;
		unsigned int	longitude;
	}me;
	unsigned int	pi;
}ADSB_POS;


typedef struct __adsb_field
{
	const char*		name;
	unsigned char*	buffer;
	unsigned int	buffer_length;
}ADSB_FIELD, *PADSB_FILED;

ADSB_POS g_adsb_pos = { 17, 0, 0, {11, 0, 0, 0, 1, 0, 0, 0}, 0 };
ADSB_FIELD g_adsb_field[] = {
	{ "DF",				(unsigned char*)&(g_adsb_pos.df),			SIZE_ADSB_POS_DF},
	{ "CA",				(unsigned char*)&(g_adsb_pos.ca),			SIZE_ADSB_POS_DF},
	{ "AA",				(unsigned char*)&(g_adsb_pos.aa),			SIZE_ADSB_POS_DF},
	{ "ME_TYPE",		(unsigned char*)&(g_adsb_pos.me.type),		SIZE_ADSB_POS_DF},
	{ "ME_MONITOR",		(unsigned char*)&(g_adsb_pos.me.monitor),	SIZE_ADSB_POS_DF},
	{ "ME_ANTENNA",		(unsigned char*)&(g_adsb_pos.me.antenna),	SIZE_ADSB_POS_DF},
	{ "ME_ALTITUDE",	(unsigned char*)&(g_adsb_pos.me.altitude),	SIZE_ADSB_POS_DF},
	{ "ME_TIME",		(unsigned char*)&(g_adsb_pos.me.time),		SIZE_ADSB_POS_DF},
	{ "ME_CPR",			(unsigned char*)&(g_adsb_pos.me.cpr),		SIZE_ADSB_POS_DF},
	{ "ME_LATITUDE",	(unsigned char*)&(g_adsb_pos.me.latitude),	SIZE_ADSB_POS_ME_LATITUDE },
	{ "ME_LONGITUDE",	(unsigned char*)&(g_adsb_pos.me.longitude),	SIZE_ADSB_POS_ME_LONGITUDE },
	{ "PI",				(unsigned char*)&(g_adsb_pos.pi),			SIZE_ADSB_POS_PI },

};
unsigned int g_adsb_pos_length;

unsigned int CPR_AIR_XZ, CPR_AIR_YZ;
//void adsb_cpr_encode_air(float wei, float jing, char i)
//{
//	//input interface
//	float lat;//weidu
//	float lon;//jingdu
//
//	//registers
//	float Dlat;//6.000000(i=0) or 6.101695(i=1)
//	float Dlon;
//	float Rlat;
//	unsigned char NL;
//	double a;
//
//	lat = wei;
//	lon = jing;
//
//	/************************CPR Encoder jing wei begin*******************************/
//	if (i == 0) Dlat = 6.000000; else Dlat = 6.101695;
//
//	CPR_AIR_YZ = floor(pow(2, 17)*(my_mod(lat, Dlat) / Dlat) + 0.5);
//	a = (double)CPR_AIR_YZ;
//	Rlat = Dlat*((a / pow(2, 17)) + floor(lat / Dlat));
//
//	if (Rlat == 0) NL = 59;
//	else if (Rlat == 87) NL = 2;
//	else if (Rlat > 87) NL = 1;
//	else if (Rlat == -87) NL = 2;
//	else if (Rlat < -87) NL = 1;
//	else NL = floor((2 * M_PI) / (acos(1 - ((1 - cos(M_PI / 30)) / (cos(HD*Rlat)*cos(HD*Rlat))))));
//
//	if (NL - i == 0) Dlon = (float)360;
//	else if (NL - i > 0) Dlon = (float)360 / (float)(NL - i);
//
//	CPR_AIR_XZ = floor(pow(2, 17)*(my_mod(lon, Dlon) / Dlon) + 0.5);
//
//	CPR_AIR_XZ = my_mod(CPR_AIR_XZ, pow(2, 17));
//	CPR_AIR_YZ = my_mod(CPR_AIR_YZ, pow(2, 17));
//
//	/************************CPR Encoder jing wei end*******************************/
//
//}



/*************   public   ***************************/
bool adsb_encoder_init(void)
{
	int i = 0;

	g_adsb_pos_length = 0;
	for (i = 0; i < sizeof(g_adsb_field)/sizeof(ADSB_FIELD); i++)
	{
		g_adsb_pos_length += g_adsb_field[i].buffer_length;
	}

	return true;
}


bool adsb_endode(float longitude, float latitude, bool *enc_buffer, unsigned int *enc_buffer_size)
{
	if (enc_buffer == NULL || 
		enc_buffer_size == NULL ||
		*enc_buffer_size < g_adsb_pos_length)	
		return false;
	

	//bit MSB/LSB translate and manchester code
	{
		int iFiled = 0;
		int iBit = 0;
		int count = 0;

		for (iFiled = 0; iFiled < sizeof(g_adsb_field)/sizeof(ADSB_FIELD); iFiled++)
		{
			PADSB_FILED pField = &(g_adsb_field[iFiled]);

			for (iBit = pField->buffer_length-1; iBit >= 0; iBit--)
			{
				if (pField->buffer[iBit / 8] && (1 << (iBit % 8)))	{ enc_buffer[count++] = 1; enc_buffer[count++] = 0; }
				else												{ enc_buffer[count++] = 0; enc_buffer[count++] = 1; }
			}
		}
	}

	return true;
}



