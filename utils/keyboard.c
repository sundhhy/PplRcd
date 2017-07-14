#include "keyboard.h"
#include "sdhDef.h"
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
const uint32_t	arr_keyGpioID[ NUM_KEYS] = { KEYGPIOID_RIGHT, KEYGPIOID_LEFT, \
	KEYGPIOID_UP, KEYGPIOID_DOWN, KEYGPIOID_ENTER, KEYGPIOID_ESC};

const uint8_t	arr_keyCode[ NUM_KEYS] = { KEYCODE_RIGHT, KEYCODE_LEFT, \
	KEYCODE_UP, KEYCODE_DOWN, KEYCODE_ENTER, KEYCODE_ESC};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static int first = 1;

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
/* Cycle/Sync Callback functions */

static int Keyboard_init( Keyboard *self, IN void *arg);
static int Key_addOb( Keyboard *self, keyObservice *ob);
static int Key_DelOb( Keyboard *self, char obId);
static void	Key_Run( Keyboard *self);
static void	Key_notify( Keyboard *self);
	
static int Key_TestUpdate( keyObservice *self, uint8_t keyEven, char numKey, uint8_t arrKeyCode[]);
static void KeyTest_setKeyHdl( KbTestOb *self, keyHdl hdl);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Keyboard *GetKeyInsance( void)
{
	static Keyboard *signalKey = NULL;
	if( signalKey == NULL)
		signalKey = Keyboard_new();
	
	return signalKey;
	
}

CTOR( Keyboard)
FUNCTION_SETTING( init, Keyboard_init);
FUNCTION_SETTING( addOb, Key_addOb);
FUNCTION_SETTING( delOb, Key_DelOb);
FUNCTION_SETTING( run, Key_Run);
FUNCTION_SETTING( notify, Key_notify);

END_CTOR

CTOR( KbTestOb)
FUNCTION_SETTING( keyObservice.update, Key_TestUpdate);
FUNCTION_SETTING( setKeyHdl, KeyTest_setKeyHdl);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static int Keyboard_init( Keyboard *self, IN void *arg)
{
	
	int i ;
	if( first == 0)
		return ERR_BEEN_INSTALL;
	
	for( i = 0; i < NUM_KEYS; i++)
	{
		if( Dev_open( arr_keyGpioID[ i], ( void *)&self->arr_p_devGpio[ i]))
			goto err;
		
	}
	for( i = 0; i < MAX_OBS; i++)
	{
		self->arr_p_obm[i].flag = 0;
	}
	first = 0;
	return RET_OK;
	
err:
	return ERR_DEV_UNAVAILABLE;
}
//·µ»ØobserviceµÄ±àºÅ
static int Key_addOb( Keyboard *self, keyObservice *ob)
{
	int i;
	for( i = 0; i < MAX_OBS; i++)
	{
		if( self->arr_p_obm[i].flag == 0)
			break;
	}
	if( i == MAX_OBS)
		return ERR_RSU_UNAVAILABLE;
	self->arr_p_obm[i].flag = 1;
	self->arr_p_obm[i].id = i | 0x80;
	self->arr_p_obm[i].p_ob = ob;
	
	return i;
}

static int Key_DelOb( Keyboard *self, char obId)
{
	int i;
	obId &= ~ 0x80;
	for( i = 0; i < MAX_OBS; i++)
	{
		if( self->arr_p_obm[i].flag == obId)
			break;
	}
	if( i == MAX_OBS)
		return ERR_NOT_EXIST;
	self->arr_p_obm[i].flag = 0;
	self->arr_p_obm[i].id = 0;
	self->arr_p_obm[i].p_ob = NULL;
	
	return i;
}

static void	Key_Run( Keyboard *self)
{
	
	
}

static void	Key_notify( Keyboard *self)
{
	int i;
	
	for( i = 0; i < MAX_OBS; i++)
	{
		if( self->arr_p_obm[i].p_ob)
			break;
	}
	
}

static int Key_TestUpdate( keyObservice *self, uint8_t keyEven, char numKey, uint8_t arrKeyCode[])
{
	KbTestOb *cthis = SUB_PTR(self, keyObservice, KbTestOb);
	if( cthis->hdl)
		cthis->hdl( keyEven, numKey, arrKeyCode);
	
	return RET_OK;
}

static void KeyTest_setKeyHdl( KbTestOb *self, keyHdl hdl)
{
	
	self->hdl = hdl;
}

//int	UtlRtc_set( UtlRtc *self, IN struct  tm *tm)
//{
//	return RET_OK;
//	
//}


//int	UtlRtc_readReg( UtlRtc *self, IN uint8_t	reg, OUT uint8_t val[], uint8_t num)
//{
//	
//	return RET_OK;
//}
//	

//int	UtlRtc_writeReg( UtlRtc *self, IN uint8_t	reg, IN uint8_t val[], uint8_t num)
//{
//	
//	return RET_OK;
//}





