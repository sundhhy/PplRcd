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




static const char PGB_code_shade[] = { "<box ></>" };
static const char PGB_code_bar[] = { "<box ></>" };
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

//两个实心方框和一个文字提示符组成一个进度条
typedef struct {
	sheet			*p_shade;
	sheet			*p_bar;
	sheet			*p_tip_text;
}PGB_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static Progress_bar *p_PGB_self = NULL;
static PGB_t				arr_pgb[NUM_PGB];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	PGB_Init(Progress_bar *self);
static void 	PGB_Set_self(Progress_bar *self);
static uint8_t 	PGB_Build_bar(bar_object_t  *b);
static void 	PGB_Update_bar(uint8_t	bar_fd);
static void 	PGB_Delete_bar(uint8_t	bar_fd);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Progress_bar	*PGB_Get_Sington(void)
{
	
	if(p_PGB_self == NULL)
		p_PGB_self = Progress_bar_new();
	
	return p_PGB_self;
	
}


CTOR(Progress_bar)
FUNCTION_SETTING(init, PGB_Init);
FUNCTION_SETTING(set_self, PGB_Set_self);
FUNCTION_SETTING(build_bar, PGB_Build_bar);
FUNCTION_SETTING(update_bar, PGB_Update_bar);
FUNCTION_SETTING(delete_bar, PGB_Delete_bar);
//FUNCTION_SETTING(show_vaild_btn, PGB_Show_vaild_bar);
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
	

	for(i = 0; i < NUM_PGB; i++)
	{
		p_exp = ExpCreate("box");
		arr_pgb[i].p_shade = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_shade, arr_pgb[i].p_shade);
		arr_pgb[i].p_bar = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_bar, arr_pgb[i].p_bar);
		
		p_exp = ExpCreate("text");
		arr_pgb[i].p_tip_text = Sheet_alloc( p_shtctl);
		p_exp->inptSht(p_exp, (void *)PGB_code_shade, arr_pgb[i].p_tip_text);

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
	int		i = 0;
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
	
	
	
	arr_pgb[i].p_shade->area.x0 = b->bar_frm.bar_x0;
	arr_pgb[i].p_shade->area.y0 = b->bar_frm.bar_y0;
	arr_pgb[i].p_bar->area.x0 = arr_pgb[i].p_shade->area.x0;
	arr_pgb[i].p_bar->area.y0 = arr_pgb[i].p_shade->area.y0 + b->bar_frm.bar_width;
	
	arr_pgb[i].p_shade->area.x1 = b->bar_frm.bar_x0 + b->bar_frm.bar_border_width;
	arr_pgb[i].p_shade->area.y1 = b->bar_frm.bar_y0 + b->bar_frm.bar_len;
	
	if(b->bar_frm.bar_tip_text_font != 0)
		arr_pgb[i].p_tip_text->cnt.font = b->bar_frm.bar_tip_text_font;
	else
		arr_pgb[i].p_tip_text->cnt.font = DEF_FONT;
	
	switch(b->bar_frm.bar_tip_text_position)
	{
		case PGB_TIP_UP:
			
			break;
		case PGB_TIP_DOWN:
			
			break;
		case PGB_TIP_LEFT:
			
			break;
		case PGB_TIP_RIGHT:
			
			break;
		
		
	}
	

	
}
static void 	PGB_Update_bar(uint8_t	bar_fd)
{
	
	
}
static void 	PGB_Delete_bar(uint8_t	bar_fd)
{
	
	
}


