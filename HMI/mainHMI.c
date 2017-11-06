#include "mainHMI.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "focus.h"


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
#define MAINHMI_BKPICNUM		"11"
#define	MAINHMI_TITLE		"总貌画面"

//static ro_char MAIN_hmi_code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >20</>" };


//每个通道的单位
static ro_char MAIN_hmi_code_data[] = { "<text f=32 m=0 mdl=test aux=0>100</>" };
static ro_char MAIN_hmi_code_unit[] = { "<text f=16 m=0 mdl=test aux=1>m3/h</>" };
//通道报警:HH HI LI LL
static ro_char MAIN_hmi_code_alarm[] = { "<text f=16 m=0 mdl=test aux=2> </>" };

//static ro_char p_input1[] = { "<input cg=2 xali=l f=24> <text f=16  yali=m clr=blue >test1</>\
//<box clr=blue bx=126 by=30></></gr>" };
//static ro_char p_input2[] = { "<input cg=2 xali=l f=24> <text f=16  yali=m clr=blue >test2</>\
//<box clr=blue bx=126 by=30></></gr>" };
//static ro_char p_input3[] = { "<input cg=2 xali=l f=24> <text f=16  yali=m clr=blue >密码:</>\
//<box clr=blue bx=126 by=30></></gr>" };

//button 3 * 3
//static ro_char p_button[] = { "<bu cols=2 cg=2 ls=2 f=16 bkc=black clr=blue xali=m x=126 y=30 ></> " };

//const hmiAtt_t	mainHmiAtt = { 1,0, COLOUR_GRAY, CHN_ROW + 2, CHN_COL};
//static sheet *p_sheets[ CHN_ROW + 2][CHN_COL] =  {NULL};

//static sheet *arr_p_focus[ 4] =  {NULL};
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

//static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total);

static void MainHmi_InitFouse( HMI *self );

static void MainHmi_Init_chnShet(void);
static int MainHmi_Data_update(void *p_data, void *p_mdl);
static int MainHmi_Util_update(void *p_data, void *p_mdl);
static int MainHmi_Alarm_update(void *p_data, void *p_mdl);
//static void MainHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
//static void MainHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
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

void Build_ChnSheets(void)
{
	int 		i = 0;
	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
	Model			*p_mdl = NULL;
	
	p_shtctl = GetShtctl();
	
//	Bulid_ChnData(g_arr_p_chnData, (void *)MAIN_hmi_code_data, MainHmi_Data_update);
	
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		g_arr_p_chnData[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_data, g_arr_p_chnData[i]) ;
		g_arr_p_chnData[i]->update = MainHmi_Data_update;
		g_arr_p_chnData[i]->id = i;
		p_mdl = g_arr_p_chnData[i]->p_mdl;
		p_mdl->attach(p_mdl, (Observer *)g_arr_p_chnData[i]);
		
		g_arr_p_chnUtil[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_unit, g_arr_p_chnUtil[i]) ;
		g_arr_p_chnUtil[i]->id = i;
		g_arr_p_chnUtil[i]->update = MainHmi_Util_update;
		g_arr_p_chnUtil[i]->cnt.subType = TEXT_ST_UNTIL;
		p_mdl = g_arr_p_chnUtil[i]->p_mdl;
		p_mdl->attach(p_mdl, (Observer *)g_arr_p_chnUtil[i]);
		
		g_arr_p_chnAlarm[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_alarm, g_arr_p_chnAlarm[i]) ;
		g_arr_p_chnAlarm[i]->id = i;
		g_arr_p_chnAlarm[i]->update = MainHmi_Alarm_update;
		p_mdl = g_arr_p_chnAlarm[i]->p_mdl;
		p_mdl->attach(p_mdl, (Observer *)g_arr_p_chnAlarm[i]);
		
		
		
	}
	
}

CTOR( mainHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_mainHmi);
FUNCTION_SETTING( HMI.hide, MainHmiHide);
FUNCTION_SETTING( HMI.initSheet, MaininitSheet);

FUNCTION_SETTING( HMI.show, MainHmiShow);
FUNCTION_SETTING( HMI.hitHandle, MainHitHandle);

FUNCTION_SETTING( HMI.init_focus, MainHmi_InitFouse);
//FUNCTION_SETTING( HMI.clear_focus, MainHmi_ClearFocuse);
//FUNCTION_SETTING( HMI.show_focus, MainHmi_ShowFocuse);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_mainHmi( HMI *self, void *arg)
{
	HMI 			*p_cmm;
	
//	HMI 			*p_kb;
//	keyboardHMI		*keyHmi ;
//	mainHmi			*cthis = SUB_PTR( self, HMI, mainHmi);
//	ro_char 		*p_txtCnt;
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	struct SHEET 	**pp_sht;

//	short i,j;
	
	p_cmm = CreateHMI( HMI_CMM);
	p_cmm->init( p_cmm, NULL);
	self->flag = 0;
	//初始化
//	p_kb = CreateHMI( HMI_KYBRD);
//	p_kb->init( p_kb, NULL);
//	keyHmi = SUB_PTR( p_kb, HMI, keyboardHMI);
	
	
//	p_shtctl = GetShtctl();
	
//	cthis->pp_shts = &p_sheets[0][0];

	//初始化背景图片
//	p_exp = ExpCreate( "pic");
//	cthis->p_bkg = Sheet_alloc( p_shtctl);
//	p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_bkPic, cthis->p_bkg) ;
	
	
	
	
	
	//timer
//	g_p_shtTime->cnt.bkc = p_sheets[0][0]->cnt.bkc;
	

	//chn info
	
	
	
//	BuildChnInfoPic( &cthis->arr_p_sht_data[0], 6);
	
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
//	FormatSheet( &mainHmiAtt, &p_shtctl->v,cthis->pp_shts);
//	FormatSheetSub( p_sheets[1][0]);
//	FormatSheetSub( p_sheets[2][0]);
//	FormatSheetSub( p_sheets[3][0]);
	
//	p_sheets[1][0]->p_enterCmd = &keyHmi->shtCmd;
//	p_sheets[2][0]->p_enterCmd = &keyHmi->shtCmd;
//	p_sheets[3][0]->p_enterCmd = &keyHmi->shtCmd;
	
//	p_sheets[1][0]->cnt.effects = EFF_FOCUS;






//	for( i = 1; i < CHN_ROW + 1; i++) {
//		for( j = 0; j < CHN_COL; j ++) {
//			FormatSheetSub( p_sheets[i][j]);
//		}
//		
//	}
	
	//初始化焦点
//	self->init_focus(self);
	
	return RET_OK;
	
//	err:
//	Sheet_free( p_sheets[i][j]);
//	return RET_OK;
}

//static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total)
//{
//	char count, i, j, numCol;
//	char arr_col[8] = { 1, 2, 2, 2, 3, 3, 3, 4};
//	
//	numCol = arr_col[ total - 1];
//	
//	
//	count = 0;
//	for( i = 1; i < 2 + 1; i++) {
//		for( j = 0; j < numCol; j ++) {
//			
//			arr_p_sheets[i][j] = CIF_build_pic( count++, total);
//		}
//		
//	}
//	
//}
static void MainHmiHide( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	int i;
	

	
	Sheet_updown(g_p_ico_trend, -1);
	Sheet_updown(g_p_ico_digital, -1);
	Sheet_updown(g_p_ico_bar, -1);
	Sheet_updown(g_p_ico_memu, -1);
	for( i = 0; i < NUM_CHANNEL; i++) {		//
		
		Sheet_updown(g_arr_p_chnAlarm[i], -1);
		Sheet_updown(g_arr_p_chnUtil[i], -1);
		Sheet_updown(g_arr_p_chnData[i], -1);
	}
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	self->clear_focus(self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
	Focus_free(self->p_fcuu);
	
}	

static void MaininitSheet( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	int i,  h = 0;;
	
	h = 0;
	g_p_sht_bkpic->cnt.data = MAINHMI_BKPICNUM;

	g_p_sht_title->cnt.data = MAINHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(MAINHMI_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	for( i = 0; i < NUM_CHANNEL; i++) {
		Sheet_updown(g_arr_p_chnData[i], h++);
		Sheet_updown(g_arr_p_chnUtil[i], h++);
		Sheet_updown(g_arr_p_chnAlarm[i], h++);
		
	}

	Sheet_updown(g_p_ico_memu, h++);
	Sheet_updown(g_p_ico_bar, h++);
	Sheet_updown(g_p_ico_digital, h++);
	Sheet_updown(g_p_ico_trend, h++);
	
	
	MainHmi_Init_chnShet();
	self->init_focus(self);
	
}


static void	MainHmiShow( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
//	int		i = 0;
//	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
//	Dev_open( LCD_DEVID, (void *)&p_lcd);
//	p_lcd->Clear( mainHmiAtt.bkc);
//	MainHmi_Init_chnShet();
	
	Sheet_refresh(g_p_sht_bkpic);
//	Sheet_refresh( cthis->p_title);
//	self->show_focus( self, 0, 0);
}

static void	MainHitHandle( HMI *self, char *s)
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	sheet 		*p_sht;
	shtCmd		*p_cmd;
//	uint8_t		focusRow = cthis->focusRow;
//	uint8_t		focusCol = cthis->focusCol;
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = self->p_fcuu->focus_col;
	char			chgFouse = 0;
	

	if( !strcmp( s, HMIKEY_UP) )
	{

	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{

		Focus_move_left(self->p_fcuu);
		chgFouse = 1;
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{

		Focus_move_right(self->p_fcuu);
		chgFouse = 1;
	}
	
	if( chgFouse)
	{
			
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
	if( !strcmp( s, HMIKEY_ENTER))
	{
		p_sht = Focus_Get_focus(self->p_fcuu);
		if(p_sht) {
			p_cmd = p_sht->p_enterCmd;
			p_cmd->shtExcute( p_cmd, p_sht, self);
			
		}
		
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		
	}
	
}


//焦点操作
static void MainHmi_InitFouse( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
//	int			i = 0;
	
	self->p_fcuu = Focus_alloc(1, 4);
	
//	for(i = 0; i < 4; i ++) {
//		
//		
//	}
	Focus_Set_sht(self->p_fcuu,0, 0, g_p_ico_memu);
	Focus_Set_sht(self->p_fcuu,0, 1, g_p_ico_bar);
	Focus_Set_sht(self->p_fcuu,0, 2, g_p_ico_digital);
	Focus_Set_sht(self->p_fcuu,0, 3, g_p_ico_trend);
	
	Focus_Set_focus(self->p_fcuu, 0, 4);	

	
}

static void MainHmi_Init_chnShet(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	int			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_data, g_arr_p_chnData[i]) ;
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_unit, g_arr_p_chnUtil[i]) ;
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_alarm, g_arr_p_chnAlarm[i]) ;
		
		g_arr_p_chnData[i]->id = i;
		g_arr_p_chnUtil[i]->id = i;
		g_arr_p_chnAlarm[i]->id = i;
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnUtil[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnAlarm[i]->cnt.colour = arr_clrs[i];
		
		
		
		g_arr_p_chnData[i]->update = MainHmi_Data_update;
		g_arr_p_chnUtil[i]->update = MainHmi_Util_update;
		g_arr_p_chnAlarm[i]->update = MainHmi_Alarm_update;

		//这是为了初始化的时候，就能让数据得到正确的坐标
		g_arr_p_chnData[i]->update(g_arr_p_chnData[i], NULL);
		g_arr_p_chnAlarm[i]->update(g_arr_p_chnAlarm[i], NULL);
		g_arr_p_chnUtil[i]->update(g_arr_p_chnUtil[i], NULL);
	
	}
}
//static void MainHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
////	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
////	sheet  			*p_fouse = arr_p_focus[ fouse_col];
////	
////	p_fouse->cnt.effects = GP_CLR_EFF( p_fouse->cnt.effects, EFF_FOCUS);
////	Sheet_slide( p_fouse);
//	
////	sheet *p_sht = Focus_Get_sht(self->p_fcuu, fouse_row, fouse_col);
////	
////	if(p_sht == NULL)
////		return;
////	p_sht->cnt.effects = GP_CLR_EFF( p_sht->cnt.effects, EFF_FOCUS);
////	Sheet_slide( p_sht);
//}
//static void MainHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
////	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
////	sheet  			*p_fouse = arr_p_focus[ fouse_col];
////	
////	p_fouse->cnt.effects = GP_SET_EFF( p_fouse->cnt.effects, EFF_FOCUS);
////	Sheet_slide( p_fouse);
//}

static int MainHmi_Data_update(void *p_data, void *p_mdl)
{
	uint8_t		up_y = 32;
	uint8_t		right_x = 106;
	uint8_t		box_sizey = 84;		
	uint8_t		box_sizex = 106;		
	
	//到四周边界的空隙
	uint8_t		space_to_up = 		box_sizey/3;	
//	uint8_t		space_to_bottom = 	0;
//	uint8_t		space_to_left = 	0;	
	uint8_t		space_to_right = 	8;
	
	char 			i = 0, j = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
	
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

	p_sht->area.x0 = right_x +  (i ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 = up_y + j * box_sizey + space_to_up;
	
	if(Sheet_is_hide(p_sht))
		return 0;
	if(IS_HMI_HIDE(g_p_mainHmi->flag))
		return 0;
	Sheet_slide( p_sht);
	return 0;
	
}

static int MainHmi_Util_update(void *p_data, void *p_mdl)
{
	uint8_t		up_y = 32;
	uint8_t		right_x = 106;
	uint8_t		box_sizey = 84;		
	uint8_t		box_sizex = 106;		
	
	//到四周边界的空隙
//	uint8_t		space_to_up = 		0;	
	uint8_t		space_to_bottom = 	2;
//	uint8_t		space_to_left = 	0;	
	uint8_t		space_to_right = 	8;
	
	char 			i = 0, j = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
	
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

	p_sht->area.x0 = right_x +  (i ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 = up_y  + (j + 1) * box_sizey  -( sizey + space_to_bottom);
	
	if(Sheet_is_hide(p_sht))
		return 0;
	if(IS_HMI_HIDE(g_p_mainHmi->flag))
		return 0;
	Sheet_slide( p_sht);
	return 0;
	
}

static int MainHmi_Alarm_update(void *p_data, void *p_mdl)
{
	uint8_t		up_y = 32;
	uint8_t		right_x = 106;
	uint8_t		box_sizey = 84;		
	uint8_t		box_sizex = 106;		
	
	//到四周边界的空隙
	uint8_t		space_to_up = 		2;	
//	uint8_t		space_to_bottom = 	0;
//	uint8_t		space_to_left = 	0;	
	uint8_t		space_to_right = 	0;
	
	char 			i = 0, j = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
	
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

//	p_sht->area.x0 = (i ) * box_sizex + space_to_left;
	p_sht->area.x0 = right_x +  (i ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 =  up_y  + (j ) * box_sizey  + ( 0 + space_to_up);
	if(Sheet_is_hide(p_sht))
		return 0;
	if(IS_HMI_HIDE(g_p_mainHmi->flag))
		return 0;
	Sheet_slide( p_sht);
	return 0;
	
}






