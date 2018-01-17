
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_progress_bar_H__
#define __INC_Component_progress_bar_H__

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


//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
//INTERFACE(Button_receive)
//{
//	void	(*btn_recv)(Button_receive *self, uint8_t	btn_id);
//	
//};



CLASS(Progress_bar)
{
//	uint8_t		btn_id;
//	uint8_t		set_vaild_btn;
//	uint8_t		cur_focush_btn;
//	uint8_t		focus_btn_num;
//	void		*arr_p_arg[NUM_BUTTON];
	
	void 		(*init)(Progress_bar *self);
//	int			(*build_each_btn)(uint8_t	seq, uint8_t btn_type, btn_hdl bh, void *hdl_arg);
//	void		(*clean_btn)(void);
////	void		(*clean_focus)(void);
//	void		(*show_vaild_btn)(void);	
////	void		(*show_focus)(void);
//	int			(*move_focus)(uint8_t	direction);		//direct 0显示在第一个活跃按钮 1 显示在下一个 2 显示在前一个
//	void		(*hit)(void);
};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
Progress_bar	*PGB_Get_Sington(void);


#endif

