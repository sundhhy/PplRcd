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
		if(p_signalCtrlKey == NULL) while(1);
		
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
int i = 0;
	uint8_t  k[2] = {KEYCODE_NONE, KEYCODE_NONE};
	uint8_t eventCode;
	
	
	if( g_p_curHmi == NULL)
		goto exit;
	
	//最多支持2个组合按键
	if(num > 2)
		num = 2;
	
	for( i = 0; i < num; i++)
	{
		
		if(IS_LEGAL_KCD(arr_msg[i].keyCode))
			k[i] = arr_msg[i].keyCode;
		
	}
	
	if(k[0] == KEYCODE_NONE)
		goto exit;
	if(num == 2) {
		g_p_curHmi->conposeKeyHandle(g_p_curHmi, k[0], k[1]);
		goto exit;
	}
	
	eventCode = arr_msg[0].eventCode & ( ~KEYEVENT_UP);
	switch( eventCode)
	{
		case KEYEVENT_HIT:
			g_p_curHmi->hitHandle( g_p_curHmi, k[0]);
			break;	
		case KEYEVENT_DHIT:
			g_p_curHmi->dhitHandle( g_p_curHmi, k[0]);
			break;
		case KEYEVENT_LPUSH:
			g_p_curHmi->longpushHandle( g_p_curHmi, k[0]);
			break;
		
	}
	
		
	exit:	
	return RET_OK;
}
