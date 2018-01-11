//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_Button.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

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
	self->p_self = self;
	
}
static int		BTN_Build_each_btn(uint8_t	seq, uint8_t btn_type, Button_receive *br)
{
	
	
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