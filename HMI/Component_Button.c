//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_Button.h"
#include "ExpFactory.h"
#include "HMI.h"
#include "format.h"
#include "sdhDef.h"
#include "arithmetic/bit.h"
//#include "basis/assert.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define BTN_ICO_MENU			"20"
#define BTN_ICO_BAR				"21"
#define BTN_ICO_DIGITAL			"22"
#define BTN_ICO_TREND			"23"
#define BTN_ICO_PGUP			"24"
#define BTN_ICO_PGDN			"25"

#define BTN_ICO_COPY			"26"
#define BTN_ICO_STOP			"27"


#define BTN_ICO_ERASE			"28"
#define BTN_ICO_SAVE			"29"

#define BTN_ICO_CLEAN			"34"			//用于清除残留的按钮

//#define BTN_ICO_LOOP			"25"
//#define BTN_ICO_SEARCH			"24"
//4个按钮的图形代码
static ro_char *arr_btn_code[NUM_BUTTON] ={ \
	"<bu vx0=10 vy0=212 bx=49 by=25 bkc=black clr=black><pic bx=48  by=24 >20</></bu>" , \
	"<bu vx0=80 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >21</></bu>" ,\
	"<bu vx0=160 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >22</></bu>" ,\
	"<bu vx0=240 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >23</></bu>" \
};

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

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static sheet *arr_p_btn_sht[NUM_BUTTON];
static Button	*p_self;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	BTN_Init(Button *self);
static int		BTN_Build_each_btn(uint8_t	seq, uint8_t btn_type, btn_hdl bh, void *hdl_arg);
static void		BTN_Clean_btn(void);
//static void		BTN_Clean_focus(void);
//static void		BTN_Show_focus(void);
static int		BTN_Move_focus(uint8_t	direction);		
static void		BTN_Deal_hit(void);
static void		BTN_Show_vaild_btn(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Button	*BTN_Get_Sington(void)
{
	static Button *Sington = NULL;
	if(Sington == NULL)
	{
		Sington = Button_new();
		if(Sington  == NULL) while(1);
	}
	
	return Sington;
	
}


CTOR(Button)
FUNCTION_SETTING(init, BTN_Init);
FUNCTION_SETTING(build_each_btn, BTN_Build_each_btn);
FUNCTION_SETTING(clean_btn, BTN_Clean_btn);
//FUNCTION_SETTING(clean_focus, BTN_Clean_focus);
FUNCTION_SETTING(show_vaild_btn, BTN_Show_vaild_btn);
//FUNCTION_SETTING(show_focus, BTN_Show_focus);
FUNCTION_SETTING(move_focus, BTN_Move_focus);
FUNCTION_SETTING(hit, BTN_Deal_hit);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void 	BTN_Init(Button *self)
{


	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
	int			i;
		
	p_shtctl = GetShtctl();
	
	//图标初始化
	p_exp = ExpCreate( "bu");


	for(i = 0; i < NUM_BUTTON; i++)
	{
		
		arr_p_btn_sht[i] = Sheet_alloc(p_shtctl);
		p_exp->inptSht(p_exp, (void *)arr_btn_code[i], arr_p_btn_sht[i]) ;
		arr_p_btn_sht[i]->area.x1 = arr_p_btn_sht[i]->area.x0 + arr_p_btn_sht[i]->bxsize;
		arr_p_btn_sht[i]->area.y1 = arr_p_btn_sht[i]->area.y0 + arr_p_btn_sht[i]->bysize;
		arr_p_btn_sht[i]->id = SHT_BTN_ID(i);
		arr_p_btn_sht[i]->height = -1;
		FormatSheetSub(arr_p_btn_sht[i]);
		

	}


	p_self = self;
	p_self->focus_btn_num = 0xff;
	
	
}
static int		BTN_Build_each_btn(uint8_t	seq, uint8_t btn_type, btn_hdl bh, void *hdl_arg)
{
	uint8_t		*p_set = &p_self->set_vaild_btn;
	dspContent_t	*btn_pic ;
//	void		*old_arg = p_self->arr_p_arg[seq];
//	btn_hdl		old_hdl = p_self->arr_hdl[seq];
	if(seq >= NUM_BUTTON)
		return -1;
//	assert(p_self != NULL);
	
	
	if(bh)
	{
		p_self->arr_p_arg[seq] = hdl_arg;
		p_self->arr_hdl[seq] = bh;
		Set_bit(p_set, seq);
	}
	btn_pic = Button_Get_subcnt(arr_p_btn_sht[seq]);
	
	
	
	switch(btn_type)
	{
				

		case BTN_TYPE_MENU:
			btn_pic->data = BTN_ICO_MENU;
			arr_p_btn_sht[seq]->id = ICO_ID_MENU;
			break;
		case BTN_TYPE_BAR:
			btn_pic->data = BTN_ICO_BAR;
			arr_p_btn_sht[seq]->id = ICO_ID_BAR;
			break;
		
		case BTN_TYPE_DIGITAL:
			btn_pic->data = BTN_ICO_DIGITAL;
			arr_p_btn_sht[seq]->id = ICO_ID_DIGITAL;
			break;
	
		case BTN_TYPE_TREND:
			btn_pic->data = BTN_ICO_TREND;
			arr_p_btn_sht[seq]->id = ICO_ID_TREND;
			break;
		case BTN_TYPE_COPY:
			btn_pic->data = BTN_ICO_COPY;
			arr_p_btn_sht[seq]->id = ICO_ID_COPY;
			break;
		case BTN_TYPE_STOP:
			btn_pic->data = BTN_ICO_STOP;
			arr_p_btn_sht[seq]->id = ICO_ID_STOP;
			break;
		
		case BTN_TYPE_PGDN:
			btn_pic->data = BTN_ICO_PGDN;
			arr_p_btn_sht[seq]->id = ICO_ID_PGDN;
			break;
	
		case BTN_TYPE_PGUP:
			btn_pic->data = BTN_ICO_PGUP;
			arr_p_btn_sht[seq]->id = ICO_ID_PGUP;
			break;
		case BTN_TYPE_ERASE:
			btn_pic->data = BTN_ICO_ERASE;
			arr_p_btn_sht[seq]->id = ICO_ID_ERASETOOL;
			break;
		case BTN_TYPE_SAVE:
			btn_pic->data = BTN_ICO_SAVE;
			arr_p_btn_sht[seq]->id = ICO_ID_SAVE;
			break;
//		case BTN_TYPE_SEARCH:
//			btn_pic->data = BTN_ICO_SEARCH;
//			arr_p_btn_sht[seq]->id = ICO_ID_SEARCH;
//			break;	
		default:
			//只有匹配的上才清除
			if(btn_type & BTN_FLAG_CLEAN)
			{
				if(arr_p_btn_sht[seq]->id  == (btn_type & 0x7f))
				{
					
					//清除残留的图像
					Clear_bit(p_set, seq);
					arr_p_btn_sht[seq]->cnt.effects = GP_CLR_EFF(arr_p_btn_sht[seq]->cnt.effects, EFF_FOCUS);
//					arr_p_btn_sht[seq]->e_heifht = 1;
					btn_pic->data = BTN_ICO_CLEAN;
					Sheet_force_slide(arr_p_btn_sht[seq]);
//					arr_p_btn_sht[seq]->e_heifht = 0;
					
					
				}
				break;
			}
			
			//不必清除残留影像，一般是切换界面时已经清除掉了
			
			
			
			Clear_bit(p_set, seq);
			
			arr_p_btn_sht[seq]->cnt.effects = GP_CLR_EFF(arr_p_btn_sht[seq]->cnt.effects, EFF_FOCUS);
				
			
				
			
			break;
		
		
		
	}
	btn_pic->len = strlen(btn_pic->data);
	return RET_OK;
}
static void		BTN_Clean_btn(void)
{
	
	if(p_self->focus_btn_num != 0xff)
	{
		arr_p_btn_sht[p_self->focus_btn_num]->cnt.effects = \
		GP_CLR_EFF(arr_p_btn_sht[p_self->focus_btn_num]->cnt.effects, EFF_FOCUS);
		p_self->focus_btn_num = 0xff;
	}
	p_self->set_vaild_btn = 0;
	
}
//static void		BTN_Clean_focus(void)
//{
//	p_self->set_vaild_btn = 0;
//	
//}

//static void		BTN_Show_focus(void)
//{
//	
//	
//}
static void		BTN_Show_vaild_btn(void)
{
	uint8_t		*p_set = &p_self->set_vaild_btn;
	int			i = 0;
	
	for(i = 0; i < NUM_BUTTON; i++)
	{
		if(Check_bit(p_set, i))
		{
			
//			arr_p_btn_sht[i]->e_heifht = 1;
			Sheet_force_slide(arr_p_btn_sht[i]);
//			arr_p_btn_sht[i]->e_heifht = 0;
		}
		
		
	}
	
}
static int		BTN_Move_focus(uint8_t	direction)
{
	uint8_t		*p_set = &p_self->set_vaild_btn;
	uint8_t 	i = 0;
	uint8_t		old_focus = p_self->focus_btn_num;
	uint8_t		first_viald_btn = NUM_BUTTON;
	uint8_t		last_viald_btn = 0;
	
	if(p_self->set_vaild_btn == 0)
	{
		p_self->focus_btn_num = 0xff;
		return -1;
		
	}
	
	switch(direction)
	{
		case BTN_MOVE_FORWARD:
			for(i = 0; i < NUM_BUTTON; i++)
			{
				if(Check_bit(p_set, i))
				{
					if(i > p_self->focus_btn_num)
					{
						p_self->focus_btn_num = i;
						goto btn_show;
					}
					
					if(i < first_viald_btn)
						first_viald_btn = i;
				}
			}
			if(p_self->focus_btn_num == 0xff)		//从外部进入按钮区域
				p_self->focus_btn_num = first_viald_btn;
			else
				p_self->focus_btn_num = 0xff;		//超出按钮区域
			
			break;
		case BTN_MOVE_BACKWARD:
			for(i = 0; i < NUM_BUTTON; i++)
			{
				if(Check_bit(p_set, i))
				{
					if(i < old_focus)
					{
						p_self->focus_btn_num = i;
						
					}
					
					last_viald_btn = i;
				}
			}
			
			if(p_self->focus_btn_num == 0xff)		//从外部进入按钮区域
				p_self->focus_btn_num = last_viald_btn;
			
			if(p_self->focus_btn_num == old_focus)		//超出按钮区域
			{
				p_self->focus_btn_num = 0xff;
				
			}
			
			break;
		case BTN_MOVE_JUMPOUT:
			p_self->focus_btn_num = 0xff;
			break;
		
		default:
			
			return -1;
	}
	btn_show:
	
	if(p_self->focus_btn_num == old_focus)		//当只有一个有效按钮的时候，会出现这种情况
		return RET_OK;
	
	//清除旧的特效
	if(old_focus != 0xff)
	{
		if(Check_bit(p_set, old_focus))
		{
			arr_p_btn_sht[old_focus]->cnt.effects = GP_CLR_EFF(arr_p_btn_sht[old_focus]->cnt.effects, EFF_FOCUS);
//			arr_p_btn_sht[old_focus]->e_heifht = 1;
			Sheet_force_slide(arr_p_btn_sht[old_focus]);
//			arr_p_btn_sht[old_focus]->e_heifht = 0;
		}
	}
	
	//显示新的选中特效
	if(p_self->focus_btn_num == 0xff)
		return RET_FAILED;
	arr_p_btn_sht[p_self->focus_btn_num]->cnt.effects = GP_SET_EFF(arr_p_btn_sht[p_self->focus_btn_num]->cnt.effects, EFF_FOCUS);
//	arr_p_btn_sht[p_self->focus_btn_num]->e_heifht = 1;
	Sheet_force_slide(arr_p_btn_sht[p_self->focus_btn_num]);
//	arr_p_btn_sht[p_self->focus_btn_num]->e_heifht = 0;	
		return RET_OK;
	
}
static void		BTN_Deal_hit(void)
{
	if(p_self->focus_btn_num == 0xff)
		return;
	
	if(p_self->arr_hdl[p_self->focus_btn_num] == NULL)
		return;
	
	p_self->arr_hdl[p_self->focus_btn_num](p_self->arr_p_arg[p_self->focus_btn_num], \
	arr_p_btn_sht[p_self->focus_btn_num]->id);
	
}
