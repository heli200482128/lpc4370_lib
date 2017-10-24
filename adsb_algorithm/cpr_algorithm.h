#ifndef __CPR_ALGORITHM_H
#define __CPR_ALGORITHM_H

void adsb_cpr_encode_air(float gps_lat, float gps_lon, char cpr_flag,
						unsigned int &cpr_encoded_lat, unsigned int &cpr_encoded_lon);
#ifdef __DEBUG_CPR
extern float Rlat1, Rlat0, Rlon1, Rlon0;
void adsb_cpr_decode_air(unsigned int CPR_AIR_YZ0, unsigned int CPR_AIR_XZ0, unsigned int CPR_AIR_YZ1, unsigned int CPR_AIR_XZ1);
#endif

#endif
