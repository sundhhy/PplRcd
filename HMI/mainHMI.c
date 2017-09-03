#include "mainHMI.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"


//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CHN_ROW			2
#define CHN_COL			3
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_mainHmi;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
static ro_char p_title[] = { "<title bkc=blue clr=white f=24 xali=l>总貌画面</> " };


//static ro_char p_input1[] = { "<input cg=2 xali=l f=24> <text f=16  yali=m clr=blue >test1</>\
//<box clr=blue bx=126 by=30></></gr>" };
//static ro_char p_input2[] = { "<input cg=2 xali=l f=24> <text f=16  yali=m clr=blue >test2</>\
//<box clr=blue bx=126 by=30></></gr>" };
//static ro_char p_input3[] = { "<input cg=2 xali=l f=24> <text f=16  yali=m clr=blue >密码:</>\
//<box clr=blue bx=126 by=30></></gr>" };

//button 3 * 3
//static ro_char p_button[] = { "<bu cols=2 cg=2 ls=2 f=16 bkc=black clr=blue xali=m x=126 y=30 ></> " };

const hmiAtt_t	mainHmiAtt = { 1,0, COLOUR_GRAY, CHN_ROW + 2, CHN_COL};
static sheet *p_sheets[ CHN_ROW + 2][CHN_COL] =  {NULL};

static sheet *arr_p_focus[ 4] =  {NULL};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static mainHmi *signal_mainHmi;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_mainHmi( HMI *self, void *arg);
static void	MainHmiShow( HMI *self);
static void	MainHitHandle( HMI *self, char *s);
static void MainHmiHide( HMI *self );
static void MaininitSheet( HMI *self );

static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total);

static void MainHmi_InitFouse( HMI *self );
static void MainHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void MainHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
mainHmi *Get_mainHmi(void) 
{
	if( signal_mainHmi == NULL)
	{
		signal_mainHmi = mainHmi_new();
		g_p_mainHmi = SUPER_PTR( signal_mainHmi, HMI);
	}
	
	return signal_mainHmi;
}

CTOR( mainHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_mainHmi);
FUNCTION_SETTING( HMI.hide, MainHmiHide);
FUNCTION_SETTING( HMI.initSheet, MaininitSheet);

FUNCTION_SETTING( HMI.show, MainHmiShow);
FUNCTION_SETTING( HMI.hitHandle, MainHitHandle);

FUNCTION_SETTING( HMI.init_focus, MainHmi_InitFouse);
FUNCTION_SETTING( HMI.clear_focus, MainHmi_ClearFocuse);
FUNCTION_SETTING( HMI.show_focus, MainHmi_ShowFocuse);

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

	short i,j;
	
	p_cmm = CreateHMI( HMI_CMM);
	p_cmm->init( p_cmm, NULL);
	
	p_kb = CreateHMI( HMI_KYBRD);
	p_kb->init( p_kb, NULL);
	keyHmi = SUB_PTR( p_kb, HMI, keyboardHMI);
	
	
	p_shtctl = GetShtctl();
	
	cthis->pp_shts = &p_sheets[0][0];
	
	//title
	p_sheets[0][0] = Sheet_alloc( p_shtctl);
	p_txtCnt = p_title;
	p_exp = ExpCreate( "title");
	p_exp->inptSht( p_exp, (void *)p_txtCnt, p_sheets[0][0]) ;
	
	
	
	//timer
	g_p_shtTime->cnt.bkc = p_sheets[0][0]->cnt.bkc;
	g_p_shtTime->cnt.bkc = ERR_COLOUR;
	

	//chn info
	BuildChnInfoPic( p_sheets, 6);
	
	//input
//	pp_inpname = &p_sheets[1][0];
//	*pp_inpname = Sheet_alloc( p_shtctl);
//	p_txtCnt = p_input1;
//	p_exp = ExpCreate( "input");
//	p_exp->inptSht( p_exp, (void *)p_txtCnt, *pp_inpname) ;
//	( *pp_inpname)->cnt.bkc = mainHmiAtt.bkc;
//	
//	
//	
//	//input
//	p_sheets[2][0] = Sheet_alloc( p_shtctl);
//	p_txtCnt = p_input2;
//	p_exp = ExpCreate( "input");
//	p_exp->inptSht( p_exp, (void *)p_txtCnt, p_sheets[2][0]) ;
//	p_sheets[2][0]->cnt.bkc = mainHmiAtt.bkc;
//	
//	
//	
//	
//	pp_sht = &p_sheets[3][0];
//	*pp_sht = Sheet_alloc( p_shtctl);
//	p_txtCnt = p_input3;
//	p_exp = ExpCreate( "input");
//	p_exp->inptSht( p_exp, (void *)p_txtCnt, *pp_sht) ;
	
	self->initSheet( self);
	//进行排版
	FormatSheet( &mainHmiAtt, &p_shtctl->v,cthis->pp_shts);
//	FormatSheetSub( p_sheets[1][0]);
//	FormatSheetSub( p_sheets[2][0]);
//	FormatSheetSub( p_sheets[3][0]);
	
//	p_sheets[1][0]->p_enterCmd = &keyHmi->shtCmd;
//	p_sheets[2][0]->p_enterCmd = &keyHmi->shtCmd;
//	p_sheets[3][0]->p_enterCmd = &keyHmi->shtCmd;
	
//	p_sheets[1][0]->cnt.effects = EFF_FOCUS;






	for( i = 1; i < CHN_ROW + 1; i++) {
		for( j = 0; j < CHN_COL; j ++) {
			FormatSheetSub( p_sheets[i][j]);
		}
		
	}
	
	//初始化焦点
	self->init_focus(self);
	
	return RET_OK;
	
//	err:
//	Sheet_free( p_sheets[i][j]);
//	return RET_OK;
}

static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total)
{
	char count, i, j, numCol;
	char arr_col[8] = { 1, 2, 2, 2, 3, 3, 3, 4};
	
	numCol = arr_col[ total - 1];
	
	
	count = 0;
	for( i = 1; i < 2 + 1; i++) {
		for( j = 0; j < numCol; j ++) {
			
			arr_p_sheets[i][j] = CIF_build_pic( count++, total);
		}
		
	}
	
}
static void MainHmiHide( HMI *self )
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	int i, j;
	Sheet_updown( g_p_ico_trend, -1);
	Sheet_updown( g_p_ico_digital, -1);
	Sheet_updown( g_p_ico_bar, -1);
	Sheet_updown( g_p_ico_memu, -1);
	for( i = CHN_ROW + 1; i > 0; i--) {
		for( j = CHN_COL; j >= 0; j --) {
			if( p_sheets[i][j])
				Sheet_updown( p_sheets[i][j], -1);
		}
		
	}
	Sheet_updown(  p_sheets[0][0], -1);
	Sheet_updown( g_p_shtTime, -1);
	
	self->clear_focus( self, cthis->focusRow, cthis->focusCol);
	
}	

static void MaininitSheet( HMI *self )
{
	
	int i, j, h;;
	
	h = 0;
	Sheet_updown(  p_sheets[0][0], h++);
	Sheet_updown( g_p_shtTime, h++);
	
	for( i = 1; i < CHN_ROW + 1; i++) {
		for( j = 0; j < CHN_COL; j ++) {
			if( p_sheets[i][j])
				Sheet_updown( p_sheets[i][j], h++);
		}
		
	}
	Sheet_updown( g_p_ico_memu, h++);
	Sheet_updown( g_p_ico_bar, h++);
	Sheet_updown( g_p_ico_digital, h++);
	Sheet_updown( g_p_ico_trend, h++);
	
	
}


static void	MainHmiShow( HMI *self )
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
	Dev_open( LCD_DEVID, (void *)&p_lcd);
	p_lcd->Clear( mainHmiAtt.bkc);
	Sheet_refresh( *cthis->pp_shts);
	self->show_focus( self, cthis->focusRow, cthis->focusCol);
}

static void	MainHitHandle( HMI *self, char *s)
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	shtCmd		*p_cmd;
	uint8_t		focusRow = cthis->focusRow;
	uint8_t		focusCol = cthis->focusCol;
	char			chgFouse = 0;
	

	if( !strcmp( s, HMIKEY_UP) )
	{

	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{
		if( cthis->focusCol > 0)
			cthis->focusCol --;
		else
		{
			cthis->focusCol = 3;
		}
		chgFouse = 1;
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{
		cthis->focusCol ++;
		if( cthis->focusCol > 3)
			cthis->focusCol = 0;
		chgFouse = 1;
	}
	
	if( chgFouse)
	{
		
		//清除旧的焦点
////		p_sheets[ focusRow][ focusCol]->cnt.effects = GP_CLR_EFF( p_sheets[ focusRow][ focusCol]->cnt.effects, EFF_FOCUS);
////		Sheet_slide( p_sheets[ focusRow][ focusCol]);
//		arr_p_focus[ focusCol]->cnt.effects = GP_CLR_EFF( arr_p_focus[ focusCol]->cnt.effects, EFF_FOCUS);
//		Sheet_slide( arr_p_focus[ focusCol]);
//		
//		//显示新的焦点
////		p_sheets[ cthis->focusRow][ cthis->focusCol]->cnt.effects = \
////			GP_SET_EFF( p_sheets[ cthis->focusRow][ cthis->focusCol]->cnt.effects, EFF_FOCUS);
////		Sheet_slide( p_sheets[ cthis->focusRow][ cthis->focusCol]);
//		
//		arr_p_focus[ cthis->focusCol]->cnt.effects = GP_SET_EFF( arr_p_focus[ cthis->focusCol]->cnt.effects, EFF_FOCUS);
//		Sheet_slide( arr_p_focus[ cthis->focusCol]);
		
		
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, cthis->focusRow, cthis->focusCol);
		
	}
	
	if( !strcmp( s, HMIKEY_ENTER))
	{
		p_cmd = arr_p_focus[ cthis->focusCol]->p_enterCmd;
		p_cmd->shtExcute( p_cmd, arr_p_focus[ cthis->focusCol], self);
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		
	}
	
}


//焦点操作
static void MainHmi_InitFouse( HMI *self )
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	
	arr_p_focus[0] = g_p_ico_memu;
	arr_p_focus[1] = g_p_ico_bar;
	arr_p_focus[2] = g_p_ico_digital;
	arr_p_focus[3] = g_p_ico_trend;
	
	cthis->focusCol = 0;
	g_p_ico_memu->cnt.effects = GP_SET_EFF( g_p_ico_memu->cnt.effects, EFF_FOCUS);
	
}


static void MainHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	sheet  			*p_fouse = arr_p_focus[ fouse_col];
	
	p_fouse->cnt.effects = GP_CLR_EFF( p_fouse->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_fouse);
}
static void MainHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	sheet  			*p_fouse = arr_p_focus[ fouse_col];
	
	p_fouse->cnt.effects = GP_SET_EFF( p_fouse->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_fouse);
}








