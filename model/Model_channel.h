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
//#define AUX_CHN_record_MB  					0x10
//#define AUX_CHN_lower_limit  				0x10
//#define AUX_CHN_upper_limit  				0x11
//#define AUX_CHN_small_signal  				0x12
//#define AUX_CHN_K							0x13
//#define AUX_CHN_B							0x14

#define MDHCHN_CHN_NUM		0x20
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef enum {
	chnaux_record_mb = 0x10,
	chnaux_filter_ts,
	chnaux_lower_limit,
	chnaux_upper_limit,
	chnaux_small_signal,
	chnaux_k,
	chnaux_b,
	alarm_hh,
	alarm_hi,
	alarm_lo,
	alarm_ll,
	tchspt_hh,
	tchspt_hi,
	tchspt_lo,
	tchspt_ll,
	alarm_backlash
}e_chn_aux_t;
typedef enum {
	 
	
	
	
	
	
	AI_B,
	AI_E,
	AI_J,
	AI_K,	
	AI_S,
	AI_T,
	AI_Pt100,
	AI_Cu50,
	AI_0_10_mA,
	AI_4_20_mA,
	AI_0_5_V,
	AI_1_5_V,
	AI_0_20_mV,
	AI_0_100_mV,
	
	AI_0_400_ohm,
	PI_0_30_kHz,
	DI_8_30_V,
	DI_0_5_V,
	AO_4_20_mA,
	es_max,
}e_signal_t;

typedef struct {
	uint8_t			chn_NO;
	uint8_t			tag_NO;
	uint8_t			signal_type;
	uint8_t			unit;
	uint8_t			MB;			//记录容量，M
	uint8_t			filter_time_s;
	uint8_t			decimal;
	uint8_t			flag_err;		//0 无错误， 1 采样失败
	short				small_signal;
	int16_t			lower_limit, upper_limit;		//

	/***************1位小数*******************/
	
	short			k,b;
	/******************************************/
	int16_t			value;
}chn_info_t;

typedef struct {
	short				alarm_hh;
	short				alarm_hi;
	short				alarm_lo;
	short				alarm_ll;
	
	//报警输出触点
	uint8_t				touch_spot_hh;
	uint8_t				touch_spot_hi;
	uint8_t				touch_spot_lo;
	uint8_t				touch_spot_ll;
	
	uint8_t				alarm_backlash;		//报警回差  0 - 10.0%
	uint8_t				none[3];
}chn_alarm_t;
	

CLASS(Model_chn)
{
		
	EXTENDS( Model);
	chn_info_t		chni;
	chn_alarm_t		alarm;
	
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
