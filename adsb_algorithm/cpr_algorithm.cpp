/**********************************************************************************************************/
// Engineer: lerking
// Create Date: 2015/4/19
// Revision: V0.1
// Description1: CPR Encoding Algorithm
// Description1: This ads-b cpr algorithm is for ads-b in the air.
/**********************************************************************************************************/
#include <stdio.h>
#include <math.h>

#include "cpr_algorithm.h"

#define M_PI       3.14159265358979323846   // pi
#define HD (0.017453292519943295769236907684886l)

inline float my_mod(float val_1 , float val_2)
{
	float a = fmod(val_1 , val_2);
	if(a<0) a = a+ val_2 ;
	return a;
}

/********************************************************************************************/
/* Function: calculate cpr encoded latitude and longitude                                   */
/* Argument: lat            [I] - latitude in NDEG - +/-[degree][min].[sec/60](gps format)  */
/*           lon            [I] - longitude in NDEG - +/-[degree][min].[sec/60](gps format) */
/*           cpr_flag       [I] - 1: odd encoding; 0: even encoding                         */
/*           cpr_encoded_lat[O] - cpr encoded latitude   									*/
/*           cpr_encoded_lon[O] - cpr encoded longitude                                     */
/* Return  : none                                                                           */
/********************************************************************************************/
void adsb_cpr_encode_air(float gps_lat, float gps_lon, char cpr_flag,
						unsigned int &cpr_encoded_lat, unsigned int &cpr_encoded_lon)
{
	//translate +/-[degree][min].[sec/60] to +/-[degree]
	//latitude, eg. 3731.49690
	float latitude_integer = (int)(gps_lat) % 100;	//31
	float latitude_decimal = gps_lat - (int)gps_lat;	//0.49690
	float lat = (int)(gps_lat / 100) + (latitude_integer + latitude_decimal) / 60;//37.524948
	//longitude, eg. //12125.76041
	float longitude_integer = (int)(gps_lon) % 100;	//25
	float longitude_decimal = gps_lon - (int)gps_lon;	//0.76041
	float lon = (int)(gps_lon / 100) + (longitude_integer + longitude_decimal) / 60;//121.429340

	unsigned int CPR_AIR_XZ;	//longitude
	unsigned int CPR_AIR_YZ;	//latitude
		
	//registers
	float Dlat;//6.000000(i=0) or 6.101695(i=1)
	float Dlon;
	float Rlat;
	unsigned char NL;
	double a;

	/************************CPR Encoder begin*******************************/
	if (cpr_flag == 0) Dlat = 6.000000; else Dlat = 6.101695;

	CPR_AIR_YZ = floor(pow(2.0, 17)*(my_mod(lat, Dlat) / Dlat) + 0.5);
	a = (double)CPR_AIR_YZ;
	Rlat = Dlat*((a / pow(2.0, 17)) + floor(lat / Dlat));

	if (Rlat == 0) NL = 59;
	else if (Rlat == 87) NL = 2;
	else if (Rlat > 87) NL = 1;
	else if (Rlat == -87) NL = 2;
	else if (Rlat < -87) NL = 1;
	else NL = floor((2 * M_PI) / (acos(1 - ((1 - cos(M_PI / 30)) / (cos(HD*Rlat)*cos(HD*Rlat))))));

	if (NL - cpr_flag == 0) Dlon = (float)360;
	else if (NL - cpr_flag > 0) Dlon = (float)360 / (float)(NL - cpr_flag);

	CPR_AIR_XZ = floor(pow(2.0, 17)*(my_mod(lon, Dlon) / Dlon) + 0.5);

	CPR_AIR_XZ = my_mod(CPR_AIR_XZ, pow(2.0, 17));
	CPR_AIR_YZ = my_mod(CPR_AIR_YZ, pow(2.0, 17));

	/************************CPR Encoder end*******************************/
	
	cpr_encoded_lat = (unsigned int)CPR_AIR_YZ;
	cpr_encoded_lon = (unsigned int)CPR_AIR_XZ;
}

#if 0	//original code
unsigned int CPR_AIR_XZ,CPR_AIR_YZ;
void adsb_cpr_encode_air(float wei,float jing,char i)
{
	//input interface
	float lat;//weidu
	float lon;//jingdu

	//registers
	float Dlat;//6.000000(i=0) or 6.101695(i=1)
	float Dlon;
	float Rlat;
	unsigned char NL;
	double a;

	lat=wei;
	lon=jing;

	/************************CPR Encoder jing wei begin*******************************/
	if(i==0) Dlat=6.000000;else Dlat=6.101695;

	CPR_AIR_YZ=floor(pow(2,17)*(my_mod(lat,Dlat)/Dlat)+0.5);
	a=(double)CPR_AIR_YZ;
	Rlat=Dlat*((a/pow(2,17))+floor(lat/Dlat));

	if(Rlat==0) NL=59;
	else if(Rlat==87) NL=2;
	else if(Rlat>87) NL=1;
	else if(Rlat==-87) NL=2;
	else if(Rlat<-87) NL=1;
	else NL=floor((2*M_PI)/(acos(1-((1-cos(M_PI/30))/(cos(HD*Rlat)*cos(HD*Rlat))))));

	if(NL-i==0) Dlon=(float)360;
	else if(NL-i>0) Dlon=(float)360/(float)(NL-i);

	CPR_AIR_XZ=floor(pow(2,17)*(my_mod(lon,Dlon)/Dlon)+0.5);

	CPR_AIR_XZ=my_mod(CPR_AIR_XZ,pow(2,17));
	CPR_AIR_YZ=my_mod(CPR_AIR_YZ,pow(2,17));

	/************************CPR Encoder jing wei end*******************************/

}
#endif

#ifdef __DEBUG_CPR

//YZ0-偶报文纬度 XZ0-偶报文经度 YZ1-奇报文纬度 XZ1-奇报文经度
//adsb_cpr_decode_air(93000,51372,74158,50194);	//lat=52.257202 lon=3.919373
float Rlat1,Rlat0,Rlon1,Rlon0;
void adsb_cpr_decode_air(unsigned int CPR_AIR_YZ0,unsigned int CPR_AIR_XZ0,unsigned int CPR_AIR_YZ1,unsigned int CPR_AIR_XZ1)
{
	float Dlat1=0;
	float Dlat0=0;
	float Dlon1=0;
	float Dlon0=0;
	float j=0;
	float n1=0;
	float n0=0;
	unsigned char NL1=0;
	unsigned char NL0=0;
	float m=0;

	/************************CPR Decoder jing wei begin*******************************/

	//计算纬度Zone的尺寸Dlati
	Dlat0=6.000000;
	Dlat1=6.101695;

	//计算纬度索引j
	j=floor(((59.0*(float)CPR_AIR_YZ0-60.0*(float)CPR_AIR_YZ1)/131072.0)+0.5);	//2^17 = 131072

	//计算不同编码方式的纬度解码值Rlati
	Rlat0 = Dlat0 * (my_mod(j, 60.0) + (double)CPR_AIR_YZ0 / 131072.0);
	Rlat1 = Dlat1 * (my_mod(j, 59.0) + (double)CPR_AIR_YZ1 / 131072.0);

	if(Rlat0>=270) Rlat0=Rlat0-360; else if(Rlat0<=-270) Rlat0=Rlat0+360;
	if(Rlat1>=270) Rlat1=Rlat1-360; else if(Rlat1<=-270) Rlat1=Rlat1+360;

	//计算纬度数目NL(Rlat0)和NL(Rlat1)
	if(Rlat0 == 0)
		NL0 = 59;
	else if(Rlat0 == 87)
		NL0 = 2;
	else if(Rlat0 > 87)
		NL0 = 1;
	else if(Rlat0 == -87)
		NL0 = 2;
	else if(Rlat0 < -87)
		NL0 = 1;
	else
		NL0 = (int)(((2.0 * M_PI) / (acos(1 - ((1 - cos(M_PI / 30.0)) / ( pow(cos(M_PI / 180.0 * Rlat0),2)))))));

	if(Rlat1 == 0)
		NL1 = 59;
	else if(Rlat1 == 87)
		NL1 = 2;
	else if(Rlat1 > 87)
		NL1 = 1;
	else if(Rlat1 == -87)
		NL1 = 2;
	else if(Rlat1 < -87)
		NL1 = 1;
	else
		NL1 = (int)(((2.0 * M_PI) / (acos(1 - ((1 - cos(M_PI / 30.0)) / ( pow(cos(M_PI / 180.0 * Rlat1),2)))))));

	if(NL0==NL1)
	{
		//计算经度Zone的尺寸Dloni
		if((NL0-0)>1) n0 = NL0-0; else n0 = 1;
		if((NL1-1)>1) n1 = NL1-1; else n1 = 1;

		Dlon0 = 360.0 / n0;
		Dlon1 = 360.0 / n1;

		//利用纬度数目NL，计算经度索引m
		m = floor(((float)CPR_AIR_XZ0 * ((float)NL0 - 1) - (float)CPR_AIR_XZ1 * (float)NL0) / 131072.0 + 0.5);

		//计算不同编码方式的经度解码值Rloni
		Rlon0 = Dlon0 * (my_mod(m, n0) + CPR_AIR_XZ0 / 131072.0);
		Rlon1 = Dlon1 * (my_mod(m, n1) + CPR_AIR_XZ1 / 131072.0);

		if(Rlon0>=180) Rlon0=Rlon0-360; else if(Rlon0<=-180) Rlon0=Rlon0+360;
		if(Rlon1>=180) Rlon1=Rlon1-360; else if(Rlon1<=-180) Rlon1=Rlon1+360;
	}
	else
	{
		Rlat1=0;Rlat0=0;Rlon1=0;Rlon0=0;
	}

	/************************CPR Decoder jing wei end*******************************/

}

#endif

