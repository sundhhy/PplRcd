#include "CtlKey.h"
#include "HMI/HMI.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

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
static CtlKey *p_signalCtrlKey;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


static int KeyUpdate( keyObservice *self,  uint8_t num, keyMsg_t arr_msg[]);
static void CtlKey_init( Controller *self, void *arg);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
CTOR( CtlKey)
SUPER_CTOR( Controller);
FUNCTION_SETTING( Controller.init, CtlKey_init);
FUNCTION_SETTING( keyObservice.update, KeyUpdate);
END_CTOR

CtlKey *Get_CtlKey(void)
{
	if( p_signalCtrlKey == NULL)
	{
		p_signalCtrlKey = CtlKey_new();
		
	}
	return p_signalCtrlKey;
}
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void CtlKey_init( Controller *self, void *arg)
{
	CtlKey	*cthis = SUB_PTR( self, Controller, CtlKey);
	Keyboard	*p_kb  = ( Keyboard *)arg;
	keyObservice	*p_keyOb;
	
	p_keyOb = SUPER_PTR( cthis, keyObservice);
	p_kb->addOb( p_kb, p_keyOb);
	
	
}


static int KeyUpdate( keyObservice *self,  uint8_t num, keyMsg_t arr_msg[])
{
//	CtlKey *cthis = SUB_PTR(self, keyObservice, CtlKey);
	
	int i = 0;
	char  *p;
	uint8_t eventCode;
	
	
	if( g_p_curHmi == NULL)
		return RET_OK;
	
	for( i = 0; i < num; i++)
	{
		p = NULL;
		switch( arr_msg[i].keyCode)
		{
			case KEYCODE_RIGHT:
				p = HMIKEY_RIGHT;
				break;	
			case KEYCODE_LEFT:
				p = HMIKEY_LEFT;
				break;
			case KEYCODE_UP:
				p = HMIKEY_UP;
				break;
			
			case KEYCODE_DOWN:
				p = HMIKEY_DOWN;
				break;
			
			case KEYCODE_ENTER:
				p = HMIKEY_ENTER;
				break;
			case KEYCODE_ESC:
				p = HMIKEY_ESC;
				break;
			default:
				p = NULL;
				break;
			
		}
		if( p == NULL)
			continue;
		
		eventCode = arr_msg[i].eventCode & ( ~KEYEVENT_UP);
		switch( eventCode)
		{
			case KEYEVENT_HIT:
				g_p_curHmi->hitHandle( g_p_curHmi, p);
				break;	
			case KEYEVENT_DHIT:
				g_p_curHmi->dhitHandle( g_p_curHmi, p);
				break;
			case KEYEVENT_LPUSH:
				g_p_curHmi->longpushHandle( g_p_curHmi, p);
				break;
		}
		
		
	}
	
	
	return RET_OK;
}
