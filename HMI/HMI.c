#include "HMI.h"
#include <string.h>
//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T Hmi_Failed = { "HMI Failed" };

//ro_char str_endRow[] = "</row>" ;
//ro_char str_endCol[] = "</col>" ;
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
HMI *g_p_curHmi;
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

static HMI *g_p_lastHmi;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void	HmiShow( HMI *self);
static void	SwitchHMI( HMI *self, HMI *p_hmi);
static void	SwitchBack( HMI *self);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
//int Is_rowEnd( const char *str)
//{
//	if( strcmp( str, str_endRow) == 0)
//		return 1;
//	
//	return 0;
//	
//}
//int Is_colEnd( const char *str)
//{
//	if( strcmp( str, str_endCol) == 0)
//		return 1;
//	
//	return 0;
//	
//}

ABS_CTOR( HMI)
FUNCTION_SETTING( show, HmiShow);
FUNCTION_SETTING( switchHMI, SwitchHMI);
FUNCTION_SETTING( switchBack, SwitchBack);

END_ABS_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//




static void	HmiShow( HMI *self)
{
	
	
}


static void	SwitchHMI( HMI *self, HMI *p_hmi)
{
	g_p_lastHmi = g_p_curHmi;
	g_p_curHmi = p_hmi;
	self->hide(self);
	p_hmi->initSheet( p_hmi);
	p_hmi->show( p_hmi);
	
}

static void	SwitchBack( HMI *self)
{
	HMI *nowHmi = g_p_lastHmi;
	g_p_lastHmi = g_p_curHmi;
	g_p_curHmi = nowHmi;
	self->hide( self);
	nowHmi->initSheet( nowHmi);
	nowHmi->show( nowHmi);
	
}









