#ifndef __INC_Model_channel_H_
#define __INC_Model_channel_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "Model.h"
#include "system.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define AUX_CHN_lower_limit  	0x10
#define AUX_CHN_upper_limit  	0x11
#define AUX_CHN_small_signal  0x12
#define AUX_CHN_K							0x13
#define AUX_CHN_B							0x14
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef struct {
	uint8_t			chn_NO;
	uint8_t			tag_NO;
	uint8_t			signal_type;
	uint8_t			unit;
	uint8_t			MB;			//记录容量，M
	uint8_t			filter_time_s;
	short				small_signal;
	

	/***************1位小数*******************/
	int					lower_limit, upper_limit;		//
	short				k,b;
	/******************************************/
	int					value;
}chn_info_t;

CLASS(Model_chn)
{
		
	EXTENDS( Model);
	chn_info_t		chni;
	
	char 	*str_buf;
	char 	*unit_buf;
	char 	*alarm_buf;
};

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------
extern Model		*arr_p_mdl_chn[NUM_CHANNEL];
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Model_chn *Get_Mode_chn(int n);






#endif
