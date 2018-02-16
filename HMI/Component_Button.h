
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_Button_H__
#define __INC_Component_Button_H__

#include <stdint.h>
#include <string.h>
#include "lw_oopc.h"

#include "sdhDef.h"

//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define		NUM_BUTTON				4

#define		BTN_TYPE_MENU			ICO_ID_MENU
#define		BTN_TYPE_BAR			ICO_ID_BAR
#define		BTN_TYPE_DIGITAL		ICO_ID_DIGITAL
#define		BTN_TYPE_TREND			ICO_ID_TREND
#define		BTN_TYPE_COPY			ICO_ID_COPY
#define		BTN_TYPE_PGUP			ICO_ID_PGUP
#define		BTN_TYPE_PGDN			ICO_ID_PGDN
#define		BTN_TYPE_ERASE			ICO_ID_ERASETOOL
#define		BTN_TYPE_SAVE			ICO_ID_SAVE		//循环显示
//#define 	BTN_TYPE_SEARCH			ICO_ID_SEARCH
#define		BTN_TYPE_STOP			ICO_ID_STOP
#define		BTN_FLAG_CLEAN			0x80		//当按钮不需要了清除掉
#define 	BTN_TYPE_NONE			0x7f




#define BTN_MOVE_FORWARD		0
#define BTN_MOVE_BACKWARD		1
#define BTN_MOVE_JUMPOUT		2
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
//INTERFACE(Button_receive)
//{
//	void	(*btn_recv)(Button_receive *self, uint8_t	btn_id);
//	
//};


typedef void (*btn_hdl)(void *arg, uint8_t btn_id);

CLASS(Button)
{
	uint8_t		btn_id;
	uint8_t		set_vaild_btn;
	uint8_t		cur_focush_btn;
	uint8_t		focus_btn_num;
	void		*arr_p_arg[NUM_BUTTON];
	btn_hdl		arr_hdl[NUM_BUTTON];
	
	void 		(*init)(Button *self);
	int			(*build_each_btn)(uint8_t	seq, uint8_t btn_type, btn_hdl bh, void *hdl_arg);
	void		(*clean_btn)(void);
//	void		(*clean_focus)(void);
	void		(*show_vaild_btn)(void);	
//	void		(*show_focus)(void);
	int			(*move_focus)(uint8_t	direction);		//direct 0显示在第一个活跃按钮 1 显示在下一个 2 显示在前一个
	void		(*hit)(void);
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Button	*BTN_Get_Sington(void);


#endif

