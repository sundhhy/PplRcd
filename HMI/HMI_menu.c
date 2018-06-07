#include "HMI_menu.h"
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
#define NUM_BTNROW		4
#define NUM_BTNCOL		2

#define MENU_OFFSET_X		90
#define MENU_OFFSET_Y		60
#define BKG_BOUND			16
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_HMI_menu;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------



void	Hide_ico_tips(HMI *self);

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//const char menu_winCode[] =  {"<bu bx=160 by=180 cg=2 rg=4 bkc=gray xali=m clr=black></>"};;
//const char menu_bkgCode[] = {"<box  bkc=gray clr=black></>"};
//const char menu_buttonCode[] = { "<bu bkc=white clr=white ><text f=12 bkc=white xali=m yali=m clr=black>总貌画面</></>" };
//static ro_char *s_buttonText[NUM_BTNROW][NUM_BTNCOL] = { {"总貌画面", "棒图画面"}, {"数显画面", "实时趋势"}, \
//	{"信息画面", "实时趋势"}, {"累计画面", "调节画面"}};
//180216 累计画面与信息画面共用同一个信息选择界面,只是参数不同

const char win_pic1_Code[] = { "<pic vx0=0 vy0=0 >5</>" };
const char win_pic2_Code[] = { "<cpic vx0=0 vy0=0 >6</>" };

static HMI **arr_pp_targetHmi[NUM_BTNROW][NUM_BTNCOL] = { { &g_p_HMI_main, &g_p_barGhHmi}, {&g_p_dataHmi, &g_p_RLT_trendHmi}, \
	{&g_p_NewSlct_HMI, &g_p_RLT_trendHmi}, {&g_p_NewSlct_HMI, NULL}};		

//static const hmiAtt_t	menuHmiAtt = { 4,4, COLOUR_GRAY, NUM_BTNROW, NUM_BTNCOL};
//static sheet *arr_p_menu_show[ NUM_BTNROW][NUM_BTNCOL] =  {NULL};
static void SwitchToHmi( HMI *self, HMI **pp_target);
//static sheet *arr_p_menu_focus[2][4] =  {NULL};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static menuHMI *singal_menu;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_menuHMI( HMI *self, void *arg);
static void	MenuHmiShow( HMI *self);
static void	MenuHitHandle( HMI *self, char kcd);
static void MenuHmiHide( HMI *self );
static void MenuinitSheet( HMI *self, uint32_t att );
static void	MNN_Run( HMI *self);

//static void MenuEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);
static void MenuClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void MenuShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

menuHMI *GetmenuHMI(void)
{
	if( singal_menu == NULL)
	{
		singal_menu = menuHMI_new();
		if(singal_menu  == NULL) while(1);
		g_p_HMI_menu = SUPER_PTR(singal_menu, HMI);
	}
	
	return singal_menu;
	
}

CTOR( menuHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_menuHMI);
FUNCTION_SETTING( HMI.hide, MenuHmiHide);
FUNCTION_SETTING( HMI.initSheet, MenuinitSheet);
FUNCTION_SETTING(HMI.build_component, Hide_ico_tips);

FUNCTION_SETTING( HMI.show, MenuHmiShow);
FUNCTION_SETTING( HMI.hitHandle, MenuHitHandle);
FUNCTION_SETTING( HMI.clear_focus, MenuClearFocuse);
FUNCTION_SETTING( HMI.show_focus, MenuShowFocuse);
//FUNCTION_SETTING(HMI.hmi_run, HMI_Run_none);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_menuHMI( HMI *self, void *arg)
{
	menuHMI			*cthis = SUB_PTR( self, HMI, menuHMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;

	p_shtctl = GetShtctl();
	
	
	p_exp = ExpCreate( "pic");
	cthis->p_sht_pic1 = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)win_pic1_Code, cthis->p_sht_pic1) ;
	
	cthis->p_sht_pic2 = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)win_pic2_Code, cthis->p_sht_pic2) ;
	

	
	//初始化焦点
	cthis->focusCol = 0;
	cthis->focusRow = 0;

	
	return RET_OK;

}

static void MenuHmiHide( HMI *self )
{
//	menuHMI			*cthis = SUB_PTR( self, HMI, menuHMI);
	
//	short i, j;

	
//	for( i = 0; i < NUM_BTNROW; i++) {
//		for( j = 0; j < NUM_BTNCOL; j ++) {
//			if( arr_p_menu_show[i][j])
//				Sheet_updown( arr_p_menu_show[i][j], -1);
//		}
//		
//	}
//	Sheet_updown( cthis->p_bkg, -1);
	
	
}	

static void MenuinitSheet( HMI *self, uint32_t att )
{
//	menuHMI			*cthis = SUB_PTR( self, HMI, menuHMI);
//	int i, j, h;;
//	
//	h = 0;
//	
//	Sheet_updown( cthis->p_bkg, h++);
//	for( i = 0; i < NUM_BTNROW; i++) {
//		for( j = 0; j < NUM_BTNCOL; j ++) {
//			if( arr_p_menu_show[i][j])
//				Sheet_updown( arr_p_menu_show[i][j], h++);
//		}
//		
//	}
	
	
	
}

void	Hide_ico_tips(HMI *self)
{
	CMP_tips 			*p_tips = TIP_Get_Sington();
	
	p_tips->hide_ico_tips(1);
}


static void	MenuHmiShow( HMI *self )
{
	menuHMI		*cthis = SUB_PTR( self, HMI, menuHMI);
//	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
//	Dev_open( LCD_DEVID, (void *)&p_lcd);
//	p_lcd->Clear( menuHMIAtt.bkc);
//	Sheet_refresh( *cthis->pp_shts);

//	Sheet_refresh( cthis->p_bkg);
	cthis->p_sht_pic1->p_gp->vdraw( cthis->p_sht_pic1->p_gp, &cthis->p_sht_pic1->cnt, &cthis->p_sht_pic1->area);
	self->show_focus( self, cthis->focusRow, cthis->focusCol);
	
}

static void MenuClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	menuHMI		*cthis = SUB_PTR( self, HMI, menuHMI);
	cthis->p_sht_pic1->p_gp->vdraw( cthis->p_sht_pic1->p_gp, &cthis->p_sht_pic1->cnt, &cthis->p_sht_pic1->area);
	Flush_LCD();
}
/*
	[ bu ] [ bu ]
	[ bu ] [ bu ]
	[ bu ] [ bu ]
	[ bu ] [ bu ]

*/

static void MenuShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	menuHMI		*cthis = SUB_PTR( self, HMI, menuHMI);
	
	short leftBu_1_x[2] = { 5, 165};
	
	char bu1_y = 38;
	char grap_y = 40;
	char size_y = 36;
	char size_x = 150;
	char off_x = 0;
	char off_y = 0;
	
	
	cthis->p_sht_pic2->area.x0 = leftBu_1_x[ fouse_col] ;
	cthis->p_sht_pic2->area.x1 = cthis->p_sht_pic2->area.x0 + size_x;
	
	cthis->p_sht_pic2->area.y0 = bu1_y + fouse_row * grap_y ;
	cthis->p_sht_pic2->area.y1 = cthis->p_sht_pic2->area.y0 + size_y;
	
	cthis->p_sht_pic2->area.offset_x = off_x;
	cthis->p_sht_pic2->area.offset_y = off_y;
	
	
	
	cthis->p_sht_pic2->p_gp->vdraw( cthis->p_sht_pic2->p_gp, &cthis->p_sht_pic2->cnt, &cthis->p_sht_pic2->area);
	Flush_LCD();
}

static void	MenuHitHandle( HMI *self, char kcd)
{
	menuHMI		*cthis = SUB_PTR( self, HMI, menuHMI);
//	shtCmd		*p_cmd;
	HMI 		**pp_trgtHmi;
	uint8_t		focusRow = cthis->focusRow;
	uint8_t		focusCol = cthis->focusCol;
	char			chgFouse = 0;
	char			row_max = NUM_BTNROW - 1;
	char			col_max = NUM_BTNCOL - 1;
	
	
	
	switch(kcd)
	{

			case KEYCODE_UP:
					if( cthis->focusRow > 0)
						cthis->focusRow --;
					else
					{
						cthis->focusRow = row_max;
					}
					chgFouse = 1; 
					break;
			case KEYCODE_DOWN:
					cthis->focusRow ++;
					if( cthis->focusRow >  row_max)
						cthis->focusRow = 0;
					chgFouse = 1;
					break;
			case KEYCODE_LEFT:
					if( cthis->focusCol > 0)
						cthis->focusCol --;
					else
					{
						cthis->focusCol = col_max;
					}
					chgFouse = 1; 
					break;
			case KEYCODE_RIGHT:
					cthis->focusCol ++;
					if( cthis->focusCol > col_max)
						cthis->focusCol = 0;
					chgFouse = 1; 
					break;

			case KEYCODE_ENTER:
					pp_trgtHmi = arr_pp_targetHmi[cthis->focusRow][cthis->focusCol];
		
					if(pp_trgtHmi == NULL)
						return;
						
					(*pp_trgtHmi)->arg[0] = 0;
					if(cthis->focusRow == 3 && cthis->focusCol == 0){
						(*pp_trgtHmi)->arg[0] = 1;
						
					}
					if(cthis->focusRow == 2 && cthis->focusCol == 1){
						(*pp_trgtHmi)->arg[0] = 1;
						
					}
					SwitchToHmi(self, pp_trgtHmi);
					break;		
			case KEYCODE_ESC:
					self->switchBack(self, 0);
					break;	
			
	}
	

//	if( !strcmp( s, HMIKEY_UP) )
//	{
//		if( cthis->focusRow > 0)
//			cthis->focusRow --;
//		else
//		{
//			cthis->focusRow = row_max;
//		}
//		chgFouse = 1;
//	}
//	else if( !strcmp( s, HMIKEY_DOWN) )
//	{
//		cthis->focusRow ++;
//		if( cthis->focusRow >  row_max)
//			cthis->focusRow = 0;
//		chgFouse = 1;
//	}
//	else if( !strcmp( s, HMIKEY_LEFT))
//	{
//		if( cthis->focusCol > 0)
//			cthis->focusCol --;
//		else
//		{
//			cthis->focusCol = col_max;
//		}
//		chgFouse = 1;
//	}
//	else if( !strcmp( s, HMIKEY_RIGHT))
//	{
//		cthis->focusCol ++;
//		if( cthis->focusCol > col_max)
//			cthis->focusCol = 0;
//		chgFouse = 1;
//	}
//	
//	
//	
//	if( !strcmp( s, KEYCODE_ENTER))
//	{
//		pp_trgtHmi = arr_pp_targetHmi[cthis->focusRow][cthis->focusCol];
//		
//		if(pp_trgtHmi == NULL)
//			return;
//			
//		(*pp_trgtHmi)->arg[0] = 0;
//		if(cthis->focusRow == 3 && cthis->focusCol == 0){
//			(*pp_trgtHmi)->arg[0] = 1;
//			
//		}
//		if(cthis->focusRow == 2 && cthis->focusCol == 1){
//			(*pp_trgtHmi)->arg[0] = 1;
//			
//		}
//		SwitchToHmi(self, pp_trgtHmi);
////		p_cmd = p_sheets[ cthis->focusRow][ cthis->focusCol]->p_enterCmd;
////		p_cmd->shtExcute( p_cmd, p_sheets[ cthis->focusRow][ cthis->focusCol], self);
//	}
//	if( !strcmp( s, HMIKEY_ESC))
//	{
//		self->switchBack(self, 0);
//	}
	
	
	if( chgFouse)
	{
		
		
		
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, cthis->focusRow, cthis->focusCol);

	}
	
	
}


//static void MenuEnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
//{
//	menuHMI	*cthis = SUB_PTR( self, shtCmd, menuHMI);
//	HMI		*selfHmi = SUPER_PTR( cthis, HMI);
//	HMI		*srcHmi = ( HMI *)arg;
//	
//	srcHmi->switchHMI( srcHmi, selfHmi);
//	
//}

static void SwitchToHmi( HMI *self, HMI **target)
{
	if( target)
		self->switchHMI( self, *target, 0);
}






