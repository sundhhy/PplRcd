//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_Button.h"
#include "ExpFactory.h"
#include "HMI.h"
#include "format.h"
//#include "sdhDef.h"
#include "arithmetic/bit.h"
//#include "basis/assert.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//4个按钮的图形代码
static ro_char *arr_btn_code[NUM_BUTTON] ={ \
	{"<bu vx0=10 vy0=212 bx=49 by=25 bkc=black clr=black><pic bx=48  by=24 >20</></bu>" } , \
	{"<bu vx0=80 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >21</></bu>" },\
	{"<bu vx0=160 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >22</></bu>" },\
	{"<bu vx0=240 vy0=212 bx=49 by=25 bkc=black clr=black><pic  bx=48  by=24 >23</></bu>" }\
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
static int		BTN_Build_each_btn(uint8_t	seq, uint8_t btn_type, Button_receive *br);
static void		BTN_Clean_btn(void);
static void		BTN_Clean_focus(void);
static void		BTN_Show_focus(void);
static int		BTN_Move_focus(uint8_t	position);		
static void		BTN_Deal_enter(void);


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
Button	*BTN_Get_Sington(void)
{
	static Button *Sington = NULL;
	if(Sington == NULL)
		Sington = Button_new();
	
	return Sington;
	
}


CTOR(Button)
FUNCTION_SETTING(init, BTN_Init);
FUNCTION_SETTING(build_each_btn, BTN_Build_each_btn);
FUNCTION_SETTING(clean_btn, BTN_Clean_btn);
FUNCTION_SETTING(clean_focus, BTN_Clean_focus);
FUNCTION_SETTING(show_focus, BTN_Show_focus);
FUNCTION_SETTING(move_focus, BTN_Move_focus);
FUNCTION_SETTING(deal_enter, BTN_Deal_enter);
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
		FormatSheetSub(arr_p_btn_sht[i]);
		

	}


	p_self = self;
	
	
}
static int		BTN_Build_each_btn(uint8_t	seq, uint8_t btn_type, Button_receive *br)
{
	if(seq >= NUM_BUTTON)
		return ;
//	assert(p_self != NULL);
	
//	Set_bit(p_self->
	switch(btn_type)
	{
		
		case BTN_TYPE_MENU:
			
			break;
	
		case BTN_TYPE_COPY:
			
			break;
		defualt:
			
			break;
		
		
		
	}
	
}
static void		BTN_Clean_btn(void)
{
	
	
}
static void		BTN_Clean_focus(void)
{
	
	
}
static void		BTN_Show_focus(void)
{
	
	
}
static int		BTN_Move_focus(uint8_t	position)
{
	
	
}
static void		BTN_Deal_enter(void)
{
	
	
}
