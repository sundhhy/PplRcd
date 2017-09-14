#include "barGraphHMI.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "ModelFactory.h"


//柱状图在y坐标上，按100%显示的话是:71 -187 
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define BARHMI_NUM_BTNROW		1
#define BARHMI_NUM_BTNCOL		4


#define BARHMI_BK_PIC				0		//背景图片编号
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_barGhHmi;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static const char barhmi_code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >21</>" };
static const char barhmi_code_bar[] = { "<box bx=35 ></>" };
static const char barhmi_code_textPrcn[] = { "<text f=16 m=0 mdl=test>100</>" };



//static const hmiAtt_t	barHmiAtt = { 4,4, COLOUR_GRAY, BARHMI_NUM_BTNROW, BARHMI_NUM_BTNCOL};


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


	
static sheet  *arr_p_sht_select[BARHMI_NUM_BTNCOL];
static char		prn_buf[BARHMI_NUM_BARS][8];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_barGhHMI( HMI *self, void *arg);
static void BarHmi_InitSheet( HMI *self );
static void BarHmi_HideSheet( HMI *self );

static void	BarHmi_Show( HMI *self);


static void	BarHmi_HitHandle( HMI *self, char *s);

//焦点
static void BarHmi_InitFouse( HMI *self );
static void BarHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void BarHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);

//柱形图操作函数
static void Init_bar( barGhHMI *self);
static void Cal_bar( barGhHMI *self);


//命令
static void BarHmi_EnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

barGhHMI *Get_barGhHMI(void)
{
	static barGhHMI *singal_barHmi = NULL;
	if( singal_barHmi == NULL)
	{
		singal_barHmi = barGhHMI_new();
		g_p_barGhHmi = SUPER_PTR( singal_barHmi, HMI);
		
	}
	
	return singal_barHmi;
	
}

CTOR( barGhHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_barGhHMI);
FUNCTION_SETTING( HMI.initSheet, BarHmi_InitSheet);
FUNCTION_SETTING( HMI.hide, BarHmi_HideSheet);
FUNCTION_SETTING( HMI.show, BarHmi_Show);

FUNCTION_SETTING( HMI.hitHandle, BarHmi_HitHandle);

FUNCTION_SETTING( HMI.init_focus, BarHmi_InitFouse);
FUNCTION_SETTING( HMI.clear_focus, BarHmi_ClearFocuse);
FUNCTION_SETTING( HMI.show_focus, BarHmi_ShowFocuse);

FUNCTION_SETTING( shtCmd.shtExcute, BarHmi_EnterCmdHdl);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_barGhHMI( HMI *self, void *arg)
{
	barGhHMI		*cthis = SUB_PTR( self, HMI, barGhHMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	
	p_shtctl = GetShtctl();
	
	//初始化背景图片
	p_exp = ExpCreate( "pic");
	cthis->p_bkg = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)barhmi_code_bkPic, cthis->p_bkg) ;
	
	//初始化柱形图
	Init_bar( cthis);
	

	//初始化焦点
	self->init_focus(self);
	
	
	return RET_OK;

}



static void BarHmi_InitSheet( HMI *self )
{
	barGhHMI			*cthis = SUB_PTR( self, HMI, barGhHMI);
	int i,  h = 0;

	Sheet_updown( cthis->p_bkg, h++);
	for( i = 0; i < BARHMI_NUM_BARS; i++) {
		Sheet_updown( cthis->arr_p_barshts[i], h++);
		Sheet_updown( cthis->arr_p_sht_textPrcn[i], h++);
		Sheet_updown( cthis->arr_p_sht_unit[i], h++);
	}
	
	Sheet_updown( g_p_ico_memu, h++);
	Sheet_updown( g_p_ico_bar, h++);
	Sheet_updown( g_p_ico_digital, h++);
	Sheet_updown( g_p_ico_trend, h++);
}

static void BarHmi_HideSheet( HMI *self )
{
	barGhHMI			*cthis = SUB_PTR( self, HMI, barGhHMI);
	
	int i;
	
	self->clear_focus(self, cthis->focusRow, cthis->focusCol);
	
	Sheet_updown( g_p_ico_trend, -1);
	Sheet_updown( g_p_ico_digital, -1);
	Sheet_updown( g_p_ico_bar, -1);
	Sheet_updown( g_p_ico_memu, -1);
	
	for( i = BARHMI_NUM_BARS - 1; i >= 0; i--) {
		Sheet_updown( cthis->arr_p_sht_unit[i], -1);
		Sheet_updown( cthis->arr_p_sht_textPrcn[i], -1);
		Sheet_updown( cthis->arr_p_barshts[i], -1);
	}
	Sheet_updown( cthis->p_bkg, -1);
	
}	





static void	BarHmi_Show( HMI *self )
{
	barGhHMI		*cthis = SUB_PTR( self, HMI, barGhHMI);
	g_p_curHmi = self;
	
	
	Cal_bar( cthis);
	
	Sheet_refresh( cthis->p_bkg);
	self->show_focus( self, cthis->focusRow, cthis->focusCol);
}

static void	BarHmi_HitHandle( HMI *self, char *s)
{
	barGhHMI		*cthis = SUB_PTR( self, HMI, barGhHMI);
	shtCmd		*p_cmd;
//	HMI 		**pp_trgtHmi;
	uint8_t		focusRow = cthis->focusRow;
	uint8_t		focusCol = cthis->focusCol;
	char			chgFouse = 0;
//	char			row_max = BARHMI_NUM_BTNROW - 1;
	char			col_max = BARHMI_NUM_BTNCOL - 1;
	

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
			cthis->focusCol = col_max;
		}
		chgFouse = 1;
	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{
		cthis->focusCol ++;
		if( cthis->focusCol > col_max)
			cthis->focusCol = 0;
		chgFouse = 1;
	}
	
	if( chgFouse)
	{
		
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, cthis->focusRow, cthis->focusCol);
		
	}
	
	if( !strcmp( s, HMIKEY_ENTER))
	{
		
		p_cmd = arr_p_sht_select[ cthis->focusCol]->p_enterCmd;
		p_cmd->shtExcute( p_cmd, arr_p_sht_select[ cthis->focusCol], self);
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
}

static void BarHmi_InitFouse( HMI *self )
{
	barGhHMI		*cthis = SUB_PTR( self, HMI, barGhHMI);
	
	
	
	arr_p_sht_select[0] = g_p_ico_memu;
	arr_p_sht_select[1] = g_p_ico_bar;
	arr_p_sht_select[2] = g_p_ico_digital;
	arr_p_sht_select[3] = g_p_ico_trend;
	
	cthis->focusCol = 0;
	cthis->focusRow = 0;
	g_p_ico_memu->cnt.effects = GP_SET_EFF( g_p_ico_memu->cnt.effects, EFF_FOCUS);
	
}
//static void BarHmi_CalFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
//	barGhHMI			*cthis = SUB_PTR( self, HMI, barGhHMI);
//	
//	
//	uint8_t		vx0[BARHMI_NUM_BTNCOL] = { 10, 50, 90, 130};
//	uint8_t		vy0 = 205;
//	uint8_t		bx = 32;
//	uint8_t		by = 32;
//	uint8_t		grap = 1;	//方框要比图标大一点
//	
//	
//	
//	cthis->p_focus->area.x0 = vx0[ fouse_col] - grap;
//	cthis->p_focus->area.x1 = vx0[ fouse_col] + bx + grap;
//	cthis->p_focus->area.y0 = vy0 - grap;
//	cthis->p_focus->area.y1 = vy0 + by + grap;
//	
//	
//}

static void BarHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet  			*p_fouse = arr_p_sht_select[fouse_col];
	
	p_fouse->cnt.effects = GP_CLR_EFF( p_fouse->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_fouse);
//	p_fouse->p_gp->vdraw ( p_fouse->p_gp, &p_fouse->cnt, &p_fouse->area);
}
static void BarHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet  			*p_fouse = arr_p_sht_select[fouse_col];
	
	p_fouse->cnt.effects = GP_SET_EFF( p_fouse->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_fouse);
//	p_fouse->p_gp->vdraw ( p_fouse->p_gp, &p_fouse->cnt, &p_fouse->area);
}

static void BarHmi_EnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
{
	barGhHMI	*cthis = SUB_PTR( self, shtCmd, barGhHMI);
	HMI		*selfHmi = SUPER_PTR( cthis, HMI);
	HMI		*srcHmi = ( HMI *)arg;
	
	srcHmi->switchHMI( srcHmi, selfHmi);
	
}



// 

static void Init_bar( barGhHMI *self)
{
	
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	short 			i;
	
	self->arr_p_sht_unit = g_arr_p_chnUtil;
	
	p_shtctl = GetShtctl();

	for( i = 0; i < BARHMI_NUM_BARS; i++) {
		p_exp = ExpCreate("box");
		self->arr_p_barshts[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)barhmi_code_bar, self->arr_p_barshts[i]) ;
		
		p_exp = ExpCreate("text");
		self->arr_p_sht_textPrcn[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)barhmi_code_textPrcn, self->arr_p_sht_textPrcn[i]) ;
		self->arr_p_sht_textPrcn[i]->cnt.data = prn_buf[i];
		
		
		//todo: 后面要绑定正确的模型，而不是测试模型
//		self->arr_p_sht_textPrcn[i]->p_mdl = ModelCreate("test");
//		self->arr_p_sht_unit[i]->p_mdl = ModelCreate("test");
		
		
		
		self->arr_p_barshts[i]->cnt.bkc = arr_clrs[i];
		self->arr_p_barshts[i]->cnt.colour = arr_clrs[i];
		
		self->arr_p_sht_textPrcn[i]->cnt.colour = arr_clrs[i];
		
		self->arr_p_sht_unit[i]->cnt.colour = arr_clrs[i];
		
	}
	
	
	Cal_bar( self);
}

//竖向柱形图在屏幕上的位置计算，注意数据都是从特定的背景图片上测量得到的
static void Cal_bar( barGhHMI *self)
{
	
	uint16_t bar_vx0[BARHMI_NUM_BARS] = { 30, 78, 126, 172, 220, 268};
	
	uint16_t bar_vy1= 187;
	uint16_t max_height= 116;
	uint16_t text_vy0 = 30;
	uint16_t unit_vy0 = 42;
	
	
	uint32_t 	i = 0, j= 1000;
	uint32_t	prcn = 0;
	uint32_t	height = 0;
	
	
	for( i = 0; i < BARHMI_NUM_BARS; i++)
	{
		
		prcn = self->arr_p_sht_textPrcn[i]->p_mdl->getMdlData( self->arr_p_sht_textPrcn[i]->p_mdl, \
			self->arr_p_sht_textPrcn[i]->cnt.mdl_aux,  &j);
		height = max_height * prcn / 1000;
		
		self->arr_p_barshts[i]->area.x0 = bar_vx0[i];
		self->arr_p_barshts[i]->area.x1 = bar_vx0[i] + self->arr_p_barshts[i]->bxsize;
		self->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
		self->arr_p_barshts[i]->area.y1 = bar_vy1;
		
		 
		self->arr_p_sht_textPrcn[i]->p_mdl->to_string( self->arr_p_sht_textPrcn[i]->p_mdl, 0, self->arr_p_sht_textPrcn[i]->cnt.data);
		self->arr_p_sht_textPrcn[i]->cnt.len = strlen( self->arr_p_sht_textPrcn[i]->cnt.data);
		self->arr_p_sht_textPrcn[i]->area.x0 = bar_vx0[i];
		self->arr_p_sht_textPrcn[i]->area.y0 = text_vy0;
		
		self->arr_p_sht_unit[i]->cnt.data = \
			self->arr_p_sht_unit[i]->p_mdl->to_string( self->arr_p_sht_unit[i]->p_mdl, 1, NULL);
		self->arr_p_sht_unit[i]->cnt.len = strlen( self->arr_p_sht_unit[i]->cnt.data);
		self->arr_p_sht_unit[i]->area.x0 = bar_vx0[i];
		self->arr_p_sht_unit[i]->area.y0 = unit_vy0;
		
		
	}
	
	
}


