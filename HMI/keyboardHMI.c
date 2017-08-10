#include "keyboardHMI.h"
#include "commHMI.h"

#include <string.h>
#include "ExpFactory.h"



//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
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
#define KEYBBUFLEN		16


const char KeyboardCode[] = { "<pic vx0=0 vy0=0 > 2</oic>" };
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static keyboardHMI *singalKBHmi;
static char keybrdbuf[KEYBBUFLEN];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_kbmHmi( HMI *self, void *arg);

static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);
static void	KeyboardShow( HMI *self );
static void KBInitSheet( HMI *self );
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
keyboardHMI *GetkeyboardHMI(void)
{
	if( singalKBHmi == NULL)
	{
		singalKBHmi = keyboardHMI_new();
		
	}
	
	return singalKBHmi;
	
}


CTOR( keyboardHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_kbmHmi);
FUNCTION_SETTING( HMI.show, KeyboardShow);
FUNCTION_SETTING( HMI.initSheet, KBInitSheet);


FUNCTION_SETTING( shtCmd.shtExcute, KeyboardEnterCmdHdl);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_kbmHmi( HMI *self, void *arg)
{
	keyboardHMI	*cthis = SUB_PTR( self, HMI, keyboardHMI);
	shtctl *p_shtctl = NULL;
	Expr *p_exp ;
	
	p_shtctl = GetShtctl();
	
	
	cthis->p_shtkybrd = Sheet_alloc( p_shtctl);
	
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)KeyboardCode, cthis->p_shtkybrd) ;
	


	
	
	return RET_OK;
}

static void KBInitSheet( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	Sheet_updown( cthis->p_shtkybrd, 0);
	
}

static void	KeyboardShow( HMI *self )
{
	keyboardHMI		*cthis = SUB_PTR( self, HMI, keyboardHMI);
	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
	Dev_open( LCD_DEVID, (void *)&p_lcd);
	p_lcd->Clear( CmmHmiAtt.bkc);
	Sheet_refresh( cthis->p_shtkybrd);
}


static void KeyboardEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
{
	keyboardHMI	*cthis = SUB_PTR( self, shtCmd, keyboardHMI);
	HMI			*selfHmi = SUPER_PTR( cthis, HMI);
	HMI		*srcHmi = ( HMI *)arg;
	
	srcHmi->switchHMI( srcHmi, selfHmi);
//	selfHmi->show( selfHmi);
	
}












