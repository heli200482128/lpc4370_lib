#ifndef __ADSB_ENCODE
#define __ADSB_ENCODE

#include <stdbool.h>

#ifdef _cplusplus
extern "C"{
#endif


/*********************************************************************************************/
/* enc_buffer_size: I -- size of enc_buffer                                                  */
/*                  O -- size of buffer including encode of longitude and latitude           */
/*********************************************************************************************/
bool adsb_encoder_init(void);
bool adsb_endode(float longitude, float latitude, 
				bool *enc_buffer, unsigned int *enc_buffer_size);	

#ifdef _cplusplus
}
#endif



#endif
