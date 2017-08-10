#include "mainHMI.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"


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
static ro_char p_title[] = { "<title bkc=purple clr=blue f=24 xali=l>设置</> " };


static ro_char p_input1[] = { "<input cg=2 xali=l > <text f=16  yali=m clr=blue >test1</>\
<box clr=blue bx=126 by=30></></gr>" };
static ro_char p_input2[] = { "<input cg=2 xali=l > <text f=16  yali=m clr=blue >test2</>\
<box clr=blue bx=126 by=30></></gr>" };
static ro_char p_input3[] = { "<input cg=2 xali=l > <text f=16  yali=m clr=blue >密码:</>\
<box clr=blue bx=126 by=30></></gr>" };

//button 3 * 3
//static ro_char p_button[] = { "<bu cols=2 cg=2 ls=2 f=16 bkc=black clr=blue xali=m x=126 y=30 ></> " };

const hmiAtt_t	mainHmiAtt = { 10,1, COLOUR_BLACK, 4, 2};
static sheet *p_sheets[4][2] =  {NULL};

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_mainHmi( HMI *self, void *arg);
static void	MainHmiShow( HMI *self);
static void	MainHitHandle( HMI *self, char *s);
static void MainHmiHide( HMI *self );
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

CTOR( mainHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_mainHmi);
FUNCTION_SETTING( HMI.hide, MainHmiHide);

FUNCTION_SETTING( HMI.show, MainHmiShow);
FUNCTION_SETTING( HMI.hitHandle, MainHitHandle);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_mainHmi( HMI *self, void *arg)
{
	HMI 			*p_cmm;
	HMI 			*p_kb;
	keyboardHMI		*keyHmi ;
	mainHmi			*cthis = SUB_PTR( self, HMI, mainHmi);
	ro_char 		*p_txtCnt;
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	struct SHEET 	**pp_sht;
	struct SHEET 	**pp_shtLabel;
	struct SHEET 	**pp_inpname;

	
	
	p_cmm = CreateHMI( HMI_CMM);
	p_cmm->init( p_cmm, NULL);
	
	p_kb = CreateHMI( HMI_KYBRD);
	p_kb->init( p_kb, NULL);
	keyHmi = SUB_PTR( p_kb, HMI, keyboardHMI);
	
	
	p_shtctl = GetShtctl();
	
	cthis->pp_shts = &p_sheets[0][0];
	
	//title
	pp_shtLabel = &p_sheets[0][0];
	*pp_shtLabel = Sheet_alloc( p_shtctl);
	p_txtCnt = p_title;
	p_exp = ExpCreate( "title");
	p_exp->inptSht( p_exp, (void *)p_txtCnt, *pp_shtLabel) ;
	
	
	
	//timer
	g_p_shtTime->cnt.bkc = (*pp_shtLabel)->cnt.bkc;
	
	
	//input
	pp_inpname = &p_sheets[1][0];
	*pp_inpname = Sheet_alloc( p_shtctl);
	p_txtCnt = p_input1;
	p_exp = ExpCreate( "input");
	p_exp->inptSht( p_exp, (void *)p_txtCnt, *pp_inpname) ;
	( *pp_inpname)->cnt.bkc = mainHmiAtt.bkc;
	
	
	
	//input
	p_sheets[2][0] = Sheet_alloc( p_shtctl);
	p_txtCnt = p_input2;
	p_exp = ExpCreate( "input");
	p_exp->inptSht( p_exp, (void *)p_txtCnt, p_sheets[2][0]) ;
	p_sheets[2][0]->cnt.bkc = mainHmiAtt.bkc;
	
	
	
	
	pp_sht = &p_sheets[3][0];
	*pp_sht = Sheet_alloc( p_shtctl);
	p_txtCnt = p_input3;
	p_exp = ExpCreate( "input");
	p_exp->inptSht( p_exp, (void *)p_txtCnt, *pp_sht) ;
	
		
	Sheet_updown( *pp_shtLabel, 0);
	Sheet_updown( g_p_shtTime, 1);
	Sheet_updown( p_sheets[1][0], 2);
	Sheet_updown( p_sheets[2][0], 3);
	Sheet_updown( p_sheets[3][0], 4);
	//进行排版

	FormatSheet( &mainHmiAtt, &p_shtctl->v,cthis->pp_shts);
	FormatSheetSub( p_sheets[1][0]);
	FormatSheetSub( p_sheets[2][0]);
	FormatSheetSub( p_sheets[3][0]);
	
	p_sheets[1][0]->p_enterCmd = &keyHmi->shtCmd;
	p_sheets[2][0]->p_enterCmd = &keyHmi->shtCmd;
	p_sheets[3][0]->p_enterCmd = &keyHmi->shtCmd;
	
	p_sheets[1][0]->cnt.effects = EFF_FOUSE;
	cthis->fouseCol = 0;
	cthis->fouseRow = 1;
	
	return RET_OK;
	
//	err:
//	Sheet_free( p_sheets[i][j]);
//	return RET_OK;
}

static void MainHmiHide( HMI *self )
{
	Sheet_updown(  p_sheets[1][0], -1);
	Sheet_updown( g_p_shtTime, -1);
	Sheet_updown( p_sheets[1][0], -1);
	Sheet_updown( p_sheets[2][0], -1);
	Sheet_updown( p_sheets[3][0], -1);
	
}	


static void	MainHmiShow( HMI *self )
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
	Dev_open( LCD_DEVID, (void *)&p_lcd);
	p_lcd->Clear( mainHmiAtt.bkc);
	Sheet_refresh( *cthis->pp_shts);
}

static void	MainHitHandle( HMI *self, char *s)
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	shtCmd		*p_cmd;
	uint8_t		fouseRow = cthis->fouseRow;
	uint8_t		fouseCol = cthis->fouseCol;
	char			chgFouse = 0;
	

	if( !strcmp( s, HMIKEY_UP) )
	{
		if( cthis->fouseRow > 1)
			cthis->fouseRow --;
		else
		{
			cthis->fouseRow = 3;
		}
		chgFouse = 1;
	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		cthis->fouseRow ++;
		if( cthis->fouseRow >3)
			cthis->fouseRow = 1;
		chgFouse = 1;
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{
		
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{
		
	}
	
	if( chgFouse)
	{
		
		//清除旧的焦点
		p_sheets[ fouseRow][ fouseCol]->cnt.effects = 0;
		Sheet_slide( p_sheets[ fouseRow][ fouseCol]);
		
		//显示新的焦点
		p_sheets[ cthis->fouseRow][ cthis->fouseCol]->cnt.effects = EFF_FOUSE;
		Sheet_slide( p_sheets[ cthis->fouseRow][ cthis->fouseCol]);
	}
	
	if( !strcmp( s, HMIKEY_ENTER))
	{
		p_cmd = p_sheets[ cthis->fouseRow][ cthis->fouseCol]->p_enterCmd;
		p_cmd->shtExcute( p_cmd, p_sheets[ cthis->fouseRow][ cthis->fouseCol], self);
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		
	}
	
}









