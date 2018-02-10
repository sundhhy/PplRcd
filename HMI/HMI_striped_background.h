#ifndef _INC_Seting_HMI_H_
#define _INC_Seting_HMI_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "HMI.h"
#include "commHMI.h"
#include <stdint.h>


//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//#define HAS_PGUP		1
//#define HAS_PGDN		2

#define HMI_SBG_SELECT_NEWS		4


#define FOCUS_IN_STARTEGY		0x10


#define SET_PG_FLAG(fg, wt)	( fg = fg | wt)
#define CLR_PG_FLAG(fg, wt)	( fg &= ~wt)
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS(HMI_striped_background)
{
	EXTENDS(HMI);
	strategy_t	*p_sy;
	sheet	*p_sht_text;
	sheet	*p_sht_CUR;			//����������ʾ�Ĺ��
	sheet	*p_sht_clean;			//����������ʾ�Ĺ��
	uint32_t	entry_start_row;
	uint8_t	sub_flag;		//�Ƿ������·�ҳͼ��ı�־
//	uint8_t	f_row;
	uint8_t	f_col;
	//��g_p_ico_pgup��g_p_ico_pgdnʱΪ1;����g_p_ico_pgupΪ2;����g_p_ico_pgdnΪ3
	//�������ͬʱ���㣬���Ժ���Ĺ���Ϊ׼
	uint8_t	col_max;	

	uint8_t	col_vx0[5];		//ÿһ�е�x��ʼλ��
};




//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern strategy_t	g_sys_strategy, g_dataPrint_strategy;
extern strategy_t	g_chn_strategy, g_alarm_strategy, g_art_strategy, g_view_strategy, g_DBP_strategy;

extern strategy_t g_news_alarm, g_news_power;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
HMI_striped_background *Get_Setting_HMI(void);
void Setting_btn_hdl(void *arg, uint8_t btn_id);

#endif