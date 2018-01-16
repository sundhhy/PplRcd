
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

#define		BTN_TYPE_MENU			0
#define		BTN_TYPE_BAR			1
#define		BTN_TYPE_DIGITAL		2
#define		BTN_TYPE_TREND			3
#define		BTN_TYPE_COPY			5
#define		BTN_TYPE_PGUP			6
#define		BTN_TYPE_PGDN			7
#define 	BTN_TYPE_NONE			0xff

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

