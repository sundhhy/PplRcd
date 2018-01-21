
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_progress_bar_H__
#define __INC_Component_progress_bar_H__

#include <stdint.h>
#include <string.h>
#include "lw_oopc.h"

#include "sdhDef.h"
#include "dev_lcd.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define		NUM_PGB							4
#define		PGB_TWD_CROSS					0
#define		PGB_TWD_PARALLEL				1

#define		PGB_TIP_UP					0
#define		PGB_TIP_DOWN				1
#define		PGB_TIP_LEFT				2
#define		PGB_TIP_RIGHT				3

#define 	PGB_BAR_ALL					0xff
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
//INTERFACE(Button_receive)
//{
//	void	(*btn_recv)(Button_receive *self, uint8_t	btn_id);
//	
//};

typedef struct
{
	uint16_t		bar_x0, bar_y0, bar_len;		//左上角的起点
	uint8_t			bar_width,  bar_border_width;
	uint8_t			bar_towards;
	uint8_t			bar_tip_text_font;
	uint8_t			bar_tip_text_position;		//0 上面 1 下面 2 左边 3 右边
	uint8_t			none;
}bar_form_t;
typedef struct
{
	uint8_t			bar_col, shade_col,bar_border_col;
	uint8_t			bar_tip_text_col;			
}bar_content_t;

typedef struct
{
	bar_form_t			bar_frm;
	bar_content_t		bar_cnt;
}bar_object_t;

CLASS(Progress_bar)
{
//	uint8_t		btn_id;
	uint8_t		set_free_bar;
	uint8_t		set_vaild_bar;
	uint8_t		none[2];
//	uint8_t		cur_focush_btn;
//	uint8_t		focus_btn_num;
//	void		*arr_p_arg[NUM_BUTTON];
	
	void 		(*init)(Progress_bar *self);
	void		(*set_self)(Progress_bar *self);
	uint8_t	(*build_bar)(bar_object_t  *b);
	void		(*update_bar)(uint8_t  bar_fd, uint8_t prc);
	void		(*delete_bar)(uint8_t  bar_fd);
	void		(*show_bar)();
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

