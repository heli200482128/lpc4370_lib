#include <stdlib.h>
#include <string.h>
#include "adsb_encode.h"


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

/************************************************************************/
/* CLASS CMessage_ADSB                                                  */
/************************************************************************/
CMessage_ADSB::CMessage_ADSB() :m_list_field(NULL), m_pnode_field_end(NULL) 
{
	memset(&m_adsb_manchester, 0, sizeof(m_adsb_manchester));
	m_adsb_manchester.preample[0] = 1;
	m_adsb_manchester.preample[2] = 1;
	m_adsb_manchester.preample[7] = 1;
	m_adsb_manchester.preample[9] = 1;

}
CMessage_ADSB::~CMessage_ADSB() 
{
	while (m_list_field)
	{
		PNODE_MESSAGE_FIELD p_field = m_list_field;
		m_list_field = p_field->next;
		free(p_field);
	}
}

bool CMessage_ADSB::append_new_field(void *p_field, unsigned int field_bitsize)
{
	if (p_field == NULL || field_bitsize == 0)	return false;

	PNODE_MESSAGE_FIELD p_field_node = (PNODE_MESSAGE_FIELD)malloc(sizeof(NODE_MESSAGE_FIELD));
	if (p_field_node == NULL)	return false;

	p_field_node->p_data = p_field;
	p_field_node->data_bitsize = field_bitsize;
	p_field_node->next = NULL;

	if (m_list_field == NULL)
	{
		m_list_field = m_pnode_field_end = p_field_node;
		return true;
	}

	m_pnode_field_end->next = p_field_node;
	m_pnode_field_end = p_field_node;

	return true;
}

void CMessage_ADSB::invert_uint16(unsigned short *dBuf, unsigned short *srcBuf)
{
	int i;
	unsigned short tmp[4];
	tmp[0] = 0;
	for (i = 0; i < 16; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (15 - i);
	}
	dBuf[0] = tmp[0];
}

void CMessage_ADSB::invert_uint8(unsigned char *dBuf, unsigned char *srcBuf)
{
	int i;
	unsigned char tmp[4];
	tmp[0] = 0;
	for (i = 0; i < 8; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (7 - i);
	}
	dBuf[0] = tmp[0];
}

unsigned short CMessage_ADSB::crc16(unsigned char *buf, unsigned short length)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;

	while (length--)
	{
		wChar = *(buf++);
		invert_uint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	invert_uint16(&wCRCin, &wCRCin);
	return (wCRCin);
}

//bool CMessage_ADSB::Get_ADSBManchester(bool *p_manchester, unsigned int &manchester_length)
//{
//	if (p_manchester == NULL || manchester_length <= 0)	return false;
//
//	unsigned char adsb_data[ADSB_MANCHESTER_DATA_COUNT_MAX / 2];
//	unsigned int adsb_data_count = 0;
//	unsigned int adsb_data_bit = 0;
//
//	memset(adsb_data, 0, ADSB_MANCHESTER_DATA_COUNT_MAX / 2);
//
//	unsigned int manchester_length_tmp = 0;
//
//	//translate adsb fields, not including pi, into manchester code, and store
//	PNODE_MESSAGE_FIELD p_field = m_list_field;
//	while (p_field)
//	{
//		for (unsigned int iBit = 0; iBit < p_field->data_bitsize; iBit++)
//		{
//			if ((*(unsigned int*)(p_field->p_data)) & (1 << iBit))
//			{
//				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT0;
//				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT1;
//				adsb_data[adsb_data_count] |= 1 << adsb_data_bit;
//			}
//			else
//			{
//				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT0;
//				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT1;
//			}
//
//			if (++adsb_data_bit == 8)
//			{
//				adsb_data_count++;
//				adsb_data_bit = 0;
//			}
//		}
//
//		if (manchester_length < manchester_length_tmp + ADSB_MANCHESTER_PREAMBLE_COUNT)	return false;
//
//		p_field = p_field->next;
//
//	}
//
//	if (adsb_data_bit)	adsb_data_count++;
//
//	m_pi = crc16(adsb_data, adsb_data_count);
//	for (unsigned int iBit = 0; iBit < p_field->data_bitsize; iBit++)
//	{
//		if ((*(unsigned int*)(p_field->p_data)) & (1 << iBit))
//		{
//			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT0;
//			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT1;
//			adsb_data[adsb_data_count] |= 1 << adsb_data_bit;
//		}
//		else
//		{
//			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT0;
//			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT1;
//		}
//
//		if (++adsb_data_bit == 8)
//		{
//			adsb_data_count++;
//			adsb_data_bit = 0;
//		}
//	}
//
//	if (manchester_length < manchester_length_tmp + ADSB_MANCHESTER_PREAMBLE_COUNT)	return false;
//
//
//	manchester_length = manchester_length_tmp + ADSB_MANCHESTER_PREAMBLE_COUNT;
//
//	memmove(p_manchester, &m_adsb_manchester, manchester_length * sizeof(bool));
//
//	return true;
//
//}

bool CMessage_ADSB::Get_ADSBManchester(bool *p_manchester, unsigned int &manchester_length)
{
	if (p_manchester == NULL || manchester_length <= 0)	return false;

	unsigned char adsb_data[ADSB_MANCHESTER_DATA_COUNT_MAX / 2];
	unsigned int adsb_data_count = 0;
	unsigned int adsb_data_bit = 0;

	memset(adsb_data, 0, ADSB_MANCHESTER_DATA_COUNT_MAX / 2);

	unsigned int manchester_length_tmp = 0;

	//translate adsb fields(MSB first), not including pi, into manchester code, and store byte format into adsb_data for crc
	PNODE_MESSAGE_FIELD p_field = m_list_field;
	while (p_field)
	{
		for (int iBit = p_field->data_bitsize - 1; iBit >= 0; iBit--)
		{
			if ((*(unsigned int*)(p_field->p_data)) & (1 << iBit))
			{
				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT0;
				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT1;
				adsb_data[adsb_data_count] |= 1 << adsb_data_bit;
			}
			else
			{
				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT0;
				m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT1;
			}

			if (++adsb_data_bit == 8)
			{
				adsb_data_count++;
				adsb_data_bit = 0;
			}
		}

		if (manchester_length < manchester_length_tmp + ADSB_MANCHESTER_PREAMBLE_COUNT)	return false;

		p_field = p_field->next;

	}

	if (adsb_data_bit)	adsb_data_count++;

	//caculate crc
	m_pi = crc16(adsb_data, adsb_data_count);

	//translate field "pi" into manchester code
	for (int iBit = ADSB_MSG_PI_BITS - 1; iBit >= 0; iBit--)
	{
		if (m_pi & (1 << iBit))
		{
			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT0;
			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_TRUE_BIT1;
		}
		else
		{
			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT0;
			m_adsb_manchester.data[manchester_length_tmp++] = MANCHESTER_FALSE_BIT1;
		}

	}

	if (manchester_length < manchester_length_tmp + ADSB_MANCHESTER_PREAMBLE_COUNT)	return false;

	//copy result to output arguments
	manchester_length = manchester_length_tmp + ADSB_MANCHESTER_PREAMBLE_COUNT;

	memmove(p_manchester, &m_adsb_manchester, manchester_length * sizeof(bool));

	return true;

}

/************************************************************************/
/* CLASS CMessage_DF17                                                  */
/************************************************************************/

CMessage_DF17::CMessage_DF17():CMessage_ADSB(), CFiled_ME_ADSB()
,m_subtype(17)
{
	m_ca = 0;
	m_aa = 0;
	Init_Field_ME();

	append_new_field((void*)&m_subtype, ADSB_DF17_SUBTYPE_BITS);
	append_new_field((void*)&m_ca, ADSB_DF17_CA_BITS);
	append_new_field((void*)&m_aa, ADSB_DF17_AA_BITS);
	append_new_field((void*)&m_type_code, ADSB_ME_TYPE_CODE_BITS);
	append_new_field((void*)&m_surveillance_status, ADSB_ME_SURVEILLANCE_STATUS_BITS);
	append_new_field((void*)&m_nic_supplement_b, ADSB_ME_NIC_SUPPLEMENT_B_BITS);
	append_new_field((void*)&m_altitude, ADSB_ME_ALTITUDE_BITS);
	append_new_field((void*)&m_time, ADSB_ME_TIME_BITS);
	append_new_field((void*)&m_cpr_format, ADSB_ME_CPR_FORMAT_BITS);
	append_new_field((void*)&m_cpr_encoded_latitude, ADSB_ME_CPR_ENCODED_LATITUDE_BITS);
	append_new_field((void*)&m_cpr_encoded_longitude, ADSB_ME_CPR_ENCODED_LONGITUDE_BITS);

}
CMessage_DF17::~CMessage_DF17() 
{
}

bool CMessage_DF17::Get_ADSBManchester(bool *p_manchester, unsigned int &manchester_length)
{

	return CMessage_ADSB::Get_ADSBManchester(p_manchester, manchester_length);
}
