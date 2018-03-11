
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef __INC_Component_tips_H__
#define __INC_Component_tips_H__

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
#define NUM_TIP_ICO					3

#define TIP_ICO_IN_TITLE			0		//标题栏上的图标
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------


typedef struct
{
	uint16_t		bar_x0, bar_y0;		//左上角的起点

}tips_form_t;
typedef struct
{
	uint8_t			bar_col, shade_col,bar_border_col;
	uint8_t			bar_tip_text_col;			
}tips_content_t;



CLASS(CMP_tips)
{
	uint8_t		flag_hide_tips;		//有些界面不能显示提示图标
	uint8_t		set_ico_vaild_tip;
	uint8_t		none[2];

	
	void 		(*init)(CMP_tips *self);
	void		(*show_ico_tips)(uint8_t tips_seq, short pic_num);
	void		(*clear_ico_tips)(uint8_t tips_seq);
	void		(*hide_ico_tips)(char ctl);
//	void		(*set_self)(CMP_tips *self);
//	uint8_t		(*alloc_tips)(CMP_tips  *b);
//	void		(*update_bar)(CMP_tips  bar_fd, uint8_t prc);
//	void		(*delete_bar)(CMP_tips  bar_fd);
	void		(*show_tips)();

};
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------
CMP_tips	*TIP_Get_Sington(void);


#endif

