#ifndef _INC_Seting_HMI_H_
#define _INC_Seting_HMI_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include "HMI.h"
#include "HMI_comm.h"
#include <stdint.h>


//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//#define HAS_PGUP		1
//#define HAS_PGDN		2
#define HMI_SBG_COL_MAX		6

#define HMI_SBG_SELECT_NEWS		4
#define HMI_SBG_SELECT_ACC		5

#define HMI_SBG_SUPER_SET_ROW		0x6
#define HMI_SBG_SUPER_SET_COL		0x0

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
	sheet	*p_sht_CUR;			//在条纹中显示的光标
	sheet	*p_sht_clean;			//在条纹中显示的光标
	uint32_t	entry_start_row;
	uint8_t	sub_flag;		//是否有上下翻页图标的标志
//	uint8_t	f_row;
	uint8_t	f_col;
	//无g_p_ico_pgup，g_p_ico_pgdn时为1;若有g_p_ico_pgup为2;若有g_p_ico_pgdn为3
	//多个规则同时满足，则以后面的规则为准
	uint8_t	col_max;	

	uint8_t	col_vx0[9];		//每一列的x起始位置 数组成员要多于HMI_SBG_COL_MAX
};




//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------
extern strategy_t	g_sys_strategy, g_dataPrint_strategy;
extern strategy_t	g_chn_strategy, g_alarm_strategy, g_art_strategy, \
	g_view_strategy, g_DBP_strategy, g_set_ACC, g_stg_super;

extern strategy_t g_news_alarm, g_news_power;
extern strategy_t g_st_acc_day, g_st_acc_month;
//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
HMI_striped_background *Get_Setting_HMI(void);
void Setting_btn_hdl(void *arg, uint8_t btn_id);

#endif
