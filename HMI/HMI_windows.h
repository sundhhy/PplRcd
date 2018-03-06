#ifndef _INC_winHMI_H_
#define _INC_winHMI_H_
#include "HMI.h"
#include "HMI_comm.h"

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//窗口类型在arg[0]传递
#define	WINTYPE_TIPS					0			//提示窗口
#define	WINTYPE_ALARM					1			
#define	WINTYPE_ERROR					2	
#define	WINTYPE_MUS_BND				3
#define	WINTYPE_MUS_UNTIL			4			//单位复选框


//设置类窗口
#define	WINTYPE_SETTING					0x10
#define	WINTYPE_TIME_SET				0x10			//
#define	WINTYPE_PASSWORD_SET			0x11			//
#define	WINTYPE_PASSWORD_INPUT			0x12			//


//窗口标志在arg[1]中传递
#define WINFLAG_RETURN				1			//该标志控制窗口在确认按键中返回原界面		
#define WINFLAG_COMMIT				2
 //------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
CLASS(winHmi)
{
	EXTENDS(HMI);
	sheet  		*p_sht_bkpic;
	sheet  		*p_sht_title;
	sheet  		*p_sht_tips;
	sheet  		*p_sht_cur;
	
	void				*p_cmd_rcv;
	stategy_cmd			cmd_hdl;
	

//	sheet  		*arr_p_sht_data[NUM_CHANNEL];
//	sheet  			**pp_shts;
	uint8_t		f_row;
	uint8_t		f_col;
	uint8_t		win_type;
	uint8_t		none;
	
	
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
winHmi *Get_winHmi(void) ;


#endif
