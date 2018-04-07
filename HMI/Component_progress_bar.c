//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_progress_bar.h"
#include "ExpFactory.h"
#include "HMI.h"
#include "sdhDef.h"
#include "arithmetic/bit.h"
//#include "basis/assert.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------




static const char PGB_code_box[] = { "<box ></>" };
static const char PGB_code_tip_text[] = { "<text f=16 m=0 >0</>" };
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef void (*cal_end_by_val)(sheet *p, uint16_t len, uint8_t prc);
//两个实心方框和一个文字提示符组成一个进度条
typedef struct {
	uint8_t			toward;
	uint8_t			bar_val;
	uint16_t		bar_len;
	char			text_buf[8];
	sheet			*p_border;
	sheet			*p_shade;
	sheet			*p_bar;
	sheet			*p_tip_text;
}PGB_t;
		
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static Progress_bar *p_PGB_self = NULL;
static PGB_t				arr_pgb[NUM_PGB];
static cal_end_by_val		arr_cal[2];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	PGB_Init(Progress_bar *self);
static void 	PGB_Set_self(Progress_bar *self);
static uint8_t 	PGB_Build_bar(bar_object_t  *b);
static void 	PGB_Update_bar(uint8_t	bar_fd, uint8_t prc);
static void 	PGB_Delete_bar(uint8_t	bar_fd);
static void 	PGB_Show_bar();

static void 	PGB_Cal_cross(sheet *p, uint16_t len, uint8_t prc);
static void 	PGB_Cal_paraller(sheet *p, uint16_t len, uint8_t prc);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Progress_bar	*PGB_Get_Sington(void)
{
	
	if(p_PGB_self == NULL)
	{
		p_PGB_self = Progress_bar_new();
		if(p_PGB_self  == NULL) while(1);
	}
	
	return p_PGB_self;
	
}


CTOR(Progress_bar)
FUNCTION_SETTING(init, PGB_Init);
FUNCTION_SETTING(set_self, PGB_Set_self);
FUNCTION_SETTING(build_bar, PGB_Build_bar);
FUNCTION_SETTING(update_bar, PGB_Update_bar);
FUNCTION_SETTING(delete_bar, PGB_Delete_bar);
FUNCTION_SETTING(show_bar, PGB_Show_bar);
////FUNCTION_SETTING(show_focus, PGB_Show_focus);
//FUNCTION_SETTING(move_focus, PGB_Move_focus);
//FUNCTION_SETTING(hit, PGB_Deal_hit);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void 	PGB_Init(Progress_bar *self)
{


	shtctl 		*p_shtctl = NULL;
	Expr 			*p_exp ;
	int				i;
		
	p_shtctl = GetShtctl();
	arr_cal[0] = PGB_Cal_cross;
	arr_cal[1] = PGB_Cal_paraller;
	

	for(i = 0; i < NUM_PGB; i++)
	{
		p_exp = ExpCreate("box");
		arr_pgb[i].p_border = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_box, arr_pgb[i].p_border);
		arr_pgb[i].p_shade = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_box, arr_pgb[i].p_shade);
		arr_pgb[i].p_bar = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_box, arr_pgb[i].p_bar);
		
		p_exp = ExpCreate("text");
		arr_pgb[i].p_tip_text = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_tip_text, arr_pgb[i].p_tip_text);
		arr_pgb[i].p_tip_text->cnt.data = arr_pgb[i].text_buf;

		arr_pgb[i].p_shade->height = -1;
		arr_pgb[i].p_bar->height = -1;
		arr_pgb[i].p_tip_text->height = -1;
	}
	
	self->set_free_bar = 0xff;


	p_PGB_self = self;
	
	
}


static void 	PGB_Set_self(Progress_bar *self)
{
	p_PGB_self = self;
}

static uint8_t 	PGB_Build_bar(bar_object_t  *b)
{
	uint16_t		i = 0;
	uint16_t		tip_sizey = 70;		
	uint16_t		tip_sizex = 160;
	
	for(i = 0; i < NUM_PGB; i ++)
	{
		if(Check_bit(&p_PGB_self->set_free_bar, i))
		{
			Clear_bit(&p_PGB_self->set_free_bar, i);
			break;
			
		}
	}
	
	if(i == NUM_PGB)
		return 0xff;
	Set_bit(&p_PGB_self->set_vaild_bar, i);
	arr_pgb[i].toward = b->bar_frm.bar_towards;
	arr_pgb[i].bar_len = b->bar_frm.bar_len;
	
	arr_pgb[i].p_shade->area.x0 = b->bar_frm.bar_x0;
	arr_pgb[i].p_shade->area.y0 = b->bar_frm.bar_y0;
	
	
	
	if(b->bar_frm.bar_towards == PGB_TWD_CROSS)		
	{
		
		arr_pgb[i].p_shade->area.y1 = b->bar_frm.bar_y0 + b->bar_frm.bar_width;
		arr_pgb[i].p_shade->area.x1 = b->bar_frm.bar_x0 + b->bar_frm.bar_len;
		
		arr_pgb[i].p_bar->area.x0 = arr_pgb[i].p_shade->area.x0;
		arr_pgb[i].p_bar->area.y0 = arr_pgb[i].p_shade->area.y0 + b->bar_frm.bar_border_width;
		arr_pgb[i].p_bar->area.y1 = arr_pgb[i].p_shade->area.y0 + b->bar_frm.bar_width - b->bar_frm.bar_border_width;
	}
	else
	{
		arr_pgb[i].p_shade->area.x1 = b->bar_frm.bar_x0 + b->bar_frm.bar_width;
		arr_pgb[i].p_shade->area.y1 = b->bar_frm.bar_y0 + b->bar_frm.bar_len;
		
		arr_pgb[i].p_bar->area.y0 = arr_pgb[i].p_shade->area.y0;
		arr_pgb[i].p_bar->area.x0 = arr_pgb[i].p_shade->area.x0 + b->bar_frm.bar_border_width;
		arr_pgb[i].p_bar->area.x1 = arr_pgb[i].p_shade->area.x0 + b->bar_frm.bar_width - b->bar_frm.bar_border_width;
	}
	
	//方框就是跟底纹一样的尺寸
	arr_pgb[i].p_border->area.x0 = arr_pgb[i].p_shade->area.x0;
	arr_pgb[i].p_border->area.y0 = arr_pgb[i].p_shade->area.y0;
	arr_pgb[i].p_border->area.x1 = arr_pgb[i].p_shade->area.x1;
	arr_pgb[i].p_border->area.y1 = arr_pgb[i].p_shade->area.y1;
	
	if(b->bar_frm.bar_tip_text_font != 0)
		arr_pgb[i].p_tip_text->cnt.font = b->bar_frm.bar_tip_text_font;
	else
		arr_pgb[i].p_tip_text->cnt.font = DEF_FONT;
	
	arr_pgb[i].p_border->cnt.colour = b->bar_cnt.bar_border_col;
	arr_pgb[i].p_tip_text->cnt.colour = b->bar_cnt.bar_tip_text_col;
	arr_pgb[i].p_shade->cnt.colour = b->bar_cnt.shade_col;
	arr_pgb[i].p_bar->cnt.colour = b->bar_cnt.bar_col;
	
	arr_pgb[i].p_shade->cnt.bkc = b->bar_cnt.shade_col;
	arr_pgb[i].p_bar->cnt.bkc = b->bar_cnt.bar_col;
	
	
	
	//计算提示字符的位置
	arr_pgb[i].p_tip_text->p_gp->getSize(arr_pgb[i].p_tip_text->p_gp, arr_pgb[i].p_tip_text->cnt.font, &tip_sizex, &tip_sizey);
	tip_sizex = tip_sizex * 4;	//最长的值是100
	if(b->bar_frm.bar_towards == PGB_TWD_CROSS)		
	{
		
		arr_pgb[i].p_tip_text->area.x0 = b->bar_frm.bar_x0 + b->bar_frm.bar_len - tip_sizex;
		switch(b->bar_frm.bar_tip_text_position)
		{
			case PGB_TIP_UP:
				arr_pgb[i].p_tip_text->area.y0 = b->bar_frm.bar_y0 - tip_sizey;
				break;
			case PGB_TIP_DOWN:
				arr_pgb[i].p_tip_text->area.y0 = b->bar_frm.bar_y0 + b->bar_frm.bar_width + 2;
				break;
			case PGB_TIP_LEFT:
				//todo: 调用着负责保证左侧有足够的空间
				arr_pgb[i].p_tip_text->area.x0 = b->bar_frm.bar_x0 - tip_sizex;
				arr_pgb[i].p_tip_text->area.y0 = b->bar_frm.bar_y0 + (b->bar_frm.bar_width - tip_sizex) / 2;
				break;
			case PGB_TIP_RIGHT:
				arr_pgb[i].p_tip_text->area.x0 = b->bar_frm.bar_x0 + b->bar_frm.bar_len + 8;
				arr_pgb[i].p_tip_text->area.y0 = b->bar_frm.bar_y0 + (b->bar_frm.bar_width - tip_sizey) / 2;
				break;
			
		}
	
			
		
	}
	else
	{
		arr_pgb[i].p_tip_text->area.x0 = b->bar_frm.bar_x0 + b->bar_frm.bar_len;
		
		switch(b->bar_frm.bar_tip_text_position)
		{
			case PGB_TIP_UP:
				arr_pgb[i].p_tip_text->area.y0 = b->bar_frm.bar_y0 - tip_sizey;
				break;
			case PGB_TIP_DOWN:
				arr_pgb[i].p_tip_text->area.y0 = b->bar_frm.bar_y0 +  b->bar_frm.bar_width + 2;
				break;
			case PGB_TIP_LEFT:
				
				break;
			case PGB_TIP_RIGHT:
				
				break;
			
		}

			
		
	}
	
	arr_pgb[i].bar_val = 0;
	
	return i;
}



static void PGB_Show_bar()
{
	int	i = 0;
	for(i = 0; i < NUM_PGB; i++)
	{
		
		PGB_Update_bar(i, 0xff);		
	}
	
	
}
static void 	PGB_Update_bar(uint8_t	bar_fd, uint8_t prc)
{
	if(bar_fd >= NUM_PGB)
		return;
	if(Check_bit(&p_PGB_self->set_vaild_bar, bar_fd) == 0)
		return;

	if(prc <= 100)
		arr_pgb[bar_fd].bar_val = prc;
	else if(prc != 0xff)
		arr_pgb[bar_fd].bar_val = 100;
	
	arr_cal[arr_pgb[bar_fd].toward](arr_pgb[bar_fd].p_bar, arr_pgb[bar_fd].bar_len, arr_pgb[bar_fd].bar_val);
	
	sprintf(arr_pgb[bar_fd].text_buf, "%%%3d", arr_pgb[bar_fd].bar_val);
	arr_pgb[bar_fd].p_tip_text->cnt.len = strlen(arr_pgb[bar_fd].text_buf);
	
//	arr_pgb[bar_fd].p_shade->e_heifht = 1;
//	arr_pgb[bar_fd].p_bar->e_heifht = 1;
//	arr_pgb[bar_fd].p_tip_text->e_heifht = 1;
//	arr_pgb[bar_fd].p_border->e_heifht = 1;
	
	//todo:180217 只考虑了百分比增加的情况
	if(prc == 0)
		Sheet_slide(arr_pgb[bar_fd].p_shade);
	Sheet_force_slide(arr_pgb[bar_fd].p_bar);
	Sheet_force_slide(arr_pgb[bar_fd].p_tip_text);
	Sheet_force_slide(arr_pgb[bar_fd].p_border);
	
//	arr_pgb[bar_fd].p_shade->e_heifht = 0;
//	arr_pgb[bar_fd].p_bar->e_heifht = 0;
//	arr_pgb[bar_fd].p_tip_text->e_heifht = 0;
//	arr_pgb[bar_fd].p_border->e_heifht = 0;
	
	
}
static void 	PGB_Delete_bar(uint8_t	bar_fd)
{
	int	i = 0;
	
	for(i = 0; i < NUM_PGB; i++)
	{
		if((i == bar_fd) || (bar_fd == HMI_CMP_ALL))
		{
			Clear_bit(&p_PGB_self->set_vaild_bar, i);
			Set_bit(&p_PGB_self->set_free_bar, i);
		}	
	}
	
	
}

static void 	PGB_Cal_cross(sheet *p, uint16_t len,uint8_t prc)
{
	int		add_len = len * prc / 100;
	
	p->area.x1 = p->area.x0 + add_len;
	
}
static void 	PGB_Cal_paraller(sheet *p, uint16_t len, uint8_t prc)
{
	int		add_len = len * prc / 100;
	p->area.y1 = p->area.y0 + add_len;
}


