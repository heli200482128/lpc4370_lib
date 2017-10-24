#ifndef __ADSB_ENCODE_H
#define __ADSB_ENCODE_H

#include "cpr_algorithm.h"

#define MANCHESTER_TRUE_BIT0	1
#define MANCHESTER_TRUE_BIT1	0
#define MANCHESTER_FALSE_BIT0	0	
#define MANCHESTER_FALSE_BIT1	1

#define ADSB_MANCHESTER_PREAMBLE_COUNT	16
#define ADSB_MANCHESTER_DATA_COUNT_MAX	224 // manchester_length = bitsize*2

class CFiled_ME_ADSB
{
#define ADSB_ME_TYPE_CODE_BITS				5
#define ADSB_ME_SURVEILLANCE_STATUS_BITS	2
#define ADSB_ME_NIC_SUPPLEMENT_B_BITS		1
#define ADSB_ME_ALTITUDE_BITS				12
#define ADSB_ME_TIME_BITS					1
#define ADSB_ME_CPR_FORMAT_BITS				1
#define ADSB_ME_CPR_ENCODED_LATITUDE_BITS	17
#define ADSB_ME_CPR_ENCODED_LONGITUDE_BITS	17

public:
	CFiled_ME_ADSB() {}
	~CFiled_ME_ADSB() {}

	void Init_Field_ME()
	{
		m_type_code = 0;
		m_surveillance_status = 0;
		m_nic_supplement_b = 0;
		m_altitude = 0;
		m_time = 0;
		m_cpr_format = 0;
		m_cpr_encoded_latitude = 0;
		m_cpr_encoded_longitude = 0;
	}

	void Set_Type_Code(unsigned char arg) { m_type_code = arg; }
	void Set_Surveillance_Status(unsigned char arg) { m_surveillance_status = arg; }
	void Set_Nic_Supplement_B(unsigned char arg) { m_nic_supplement_b = arg; }
	void Set_Altitude(unsigned short arg) { m_altitude = arg; }
	void Set_Time(unsigned char arg) { m_time = arg; }
	void Set_Cpr_Format(unsigned char arg) { m_cpr_format = arg; }
	void Set_Cpr_Encoded_Latitude(unsigned int arg) { m_cpr_encoded_latitude = arg; }
	void Set_Cpr_Encoded_Longitude(unsigned int arg) { m_cpr_encoded_longitude = arg; }

	void Set_ADSB_Position(double gps_alt, double gps_lat, double gps_lon, unsigned char cpr_flag)
	{
		//check unit transformation
		Set_Altitude((unsigned int)gps_alt);

		unsigned int cpr_encoded_lat, cpr_encoded_lon;
		adsb_cpr_encode_air((float)gps_lat, (float)gps_lon, cpr_flag, cpr_encoded_lat, cpr_encoded_lon);

		Set_Cpr_Format(cpr_flag);
		Set_Cpr_Encoded_Latitude(cpr_encoded_lat);
		Set_Cpr_Encoded_Longitude(cpr_encoded_lon);
	}

protected:
	unsigned char m_type_code;			//5bits
	unsigned char m_surveillance_status;//2bits
	unsigned char m_nic_supplement_b;	//1bit
	unsigned short m_altitude;			//12bits
	unsigned char m_time;				//1bit
	unsigned char m_cpr_format;			//1bit
	unsigned int m_cpr_encoded_latitude;//17bits
	unsigned int m_cpr_encoded_longitude;//17bits


};


/************************************************************************/
/* CLASS CMessage_ADSB                                                  */
/************************************************************************/
class CMessage_ADSB
{
#define ADSB_MSG_PI_BITS	24
typedef struct __message_field
{
	void*					p_data;
	unsigned int			data_bitsize;
	struct __message_field*	next;

}NODE_MESSAGE_FIELD, *PNODE_MESSAGE_FIELD, *LIST_MESSAGE_FIELD;

public:
	CMessage_ADSB();
	~CMessage_ADSB();
	
	virtual bool Get_ADSBManchester(bool *p_manchester, unsigned int &manchester_length);


protected:

	bool append_new_field(void *p_field, unsigned int field_bitsize);

private:
	//adsb message field 'pi'
	unsigned int	m_pi;			//24bits

	//manchester structure
	struct
	{
		bool	preample[ADSB_MANCHESTER_PREAMBLE_COUNT];
		bool	data[ADSB_MANCHESTER_DATA_COUNT_MAX];
	}m_adsb_manchester;	//MSB ahead

	//list of adsb message fields
	LIST_MESSAGE_FIELD	m_list_field;// list of fields, but not include pi
	PNODE_MESSAGE_FIELD m_pnode_field_end;


	void invert_uint16(unsigned short *dBuf, unsigned short *srcBuf);
	void invert_uint8(unsigned char *dBuf, unsigned char *srcBuf);
	unsigned short crc16(unsigned char *buf, unsigned short length);

};

/************************************************************************/
/* CLASS CMessage_DF17                                                  */
/************************************************************************/

class CMessage_DF17:public CMessage_ADSB, public CFiled_ME_ADSB
{
#define ADSB_DF17_SUBTYPE_BITS	5
#define ADSB_DF17_CA_BITS		3
#define ADSB_DF17_AA_BITS		24
public:
	CMessage_DF17();
	~CMessage_DF17();

	virtual bool Get_ADSBManchester(bool *p_manchester, unsigned int &manchester_length);

	void Set_Ca(unsigned char arg) { m_ca = arg; }
	void Set_Aa(unsigned char arg) { m_aa = arg; }

private:

	const unsigned char	m_subtype;	//5bits
	unsigned char	m_ca;		//3bits
	unsigned char	m_aa;		//24bits
	// me filed here


};

#endif
