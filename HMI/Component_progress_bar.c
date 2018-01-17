//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_progress_bar.h"
#include "ExpFactory.h"
#include "HMI.h"
#include "format.h"
#include "sdhDef.h"
#include "arithmetic/bit.h"
//#include "basis/assert.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//4个按钮的图形代码
//static ro_char *arr_btn_code[NUM_BUTTON] ={ \
//	"<bu vx0=10 vy0=212 bx=49 by=25 bkc=black clr=black><pic bx=48  by=24 >20</></bu>" , \
//	"<bu vx0=80 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >21</></bu>" ,\
//	"<bu vx0=160 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >22</></bu>" ,\
//	"<bu vx0=240 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >23</></bu>" \
//};

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
static Progress_bar	*p_PGB_self;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	PGB_Init(Progress_bar *self);
static int		PGB_Build_each_btn(uint8_t	seq, uint8_t btn_type, btn_hdl bh, void *hdl_arg);
static void		PGB_Clean_btn(void);
//static void		PGB_Clean_focus(void);
//static void		PGB_Show_focus(void);
static int		PGB_Move_focus(uint8_t	direction);		
static void		PGB_Deal_hit(void);
static void		PGB_Show_vaild_bar(void);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Progress_bar	*PGB_Get_Sington(void)
{
	static Progress_bar *PGB_Sington = NULL;
	if(PGB_Sington == NULL)
		PGB_Sington = Progress_bar_new();
	
	return PGB_Sington;
	
}


CTOR(Progress_bar)
FUNCTION_SETTING(init, PGB_Init);
//FUNCTION_SETTING(build_each_btn, PGB_Build_each_btn);
//FUNCTION_SETTING(clean_btn, PGB_Clean_btn);
////FUNCTION_SETTING(clean_focus, PGB_Clean_focus);
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
	Expr 		*p_exp ;
	int			i;
		
	p_shtctl = GetShtctl();
	
	//图标初始化
	p_exp = ExpCreate( "bu");


	for(i = 0; i < NUM_BUTTON; i++)
	{
		
//		arr_p_btn_sht[i] = Sheet_alloc(p_shtctl);
//		p_exp->inptSht(p_exp, (void *)arr_btn_code[i], arr_p_btn_sht[i]) ;
//		arr_p_btn_sht[i]->area.x1 = arr_p_btn_sht[i]->area.x0 + arr_p_btn_sht[i]->bxsize;
//		arr_p_btn_sht[i]->area.y1 = arr_p_btn_sht[i]->area.y0 + arr_p_btn_sht[i]->bysize;
//		arr_p_btn_sht[i]->id = SHT_PGB_ID(i);
//		arr_p_btn_sht[i]->height = -1;
//		FormatSheetSub(arr_p_btn_sht[i]);
		

	}


	p_PGB_self = self;
	
	
}
static int		PGB_Build_each_btn(uint8_t	seq, uint8_t btn_type, btn_hdl bh, void *hdl_arg)
{
	
	return RET_OK;
}
static void		PGB_Clean_btn(void)
{
	
	
}
//static void		PGB_Clean_focus(void)
//{
//	p_PGB_self->set_vaild_btn = 0;
//	
//}

//static void		PGB_Show_focus(void)
//{
//	
//	
//}
static void		PGB_Show_vaild_bar(void)
{
//	uint8_t		*p_set = &p_PGB_self->set_vaild_btn;
//	int			i = 0;
//	
//	for(i = 0; i < NUM_BUTTON; i++)
//	{
//		if(Check_bit(p_set, i))
//		{
//			
//			arr_p_btn_sht[i]->e_heifht = 1;
//			Sheet_slide(arr_p_btn_sht[i]);
//			arr_p_btn_sht[i]->e_heifht = 0;
//		}
//		
//		
//	}
	
}
static int		PGB_Move_focus(uint8_t	direction)
{
	
	
}
static void		PGB_Deal_hit(void)
{
	
	
}
