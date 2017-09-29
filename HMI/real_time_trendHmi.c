#include "real_time_trendHmi.h"
#include "menuHMI.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "ModelFactory.h"
#include "curve.h"



//柱状图在y坐标上，按100%显示的话是:71 -187 
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------



#define REAKTIMEHMI_BK_PIC				0		//背景图片编号
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_RLT_trendHmi;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static const char RT_hmi_code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >23</>" };

static const char RT_hmi_code_title[] =  {"<text vx0=0 vy0=4 m=0 clr=white f=24>实时趋势</>" };

static const char RT_hmi_code_div[] = { "<text vx0=8 vy0=36 f=16 m=0 clr=red>1</>" };

static const char RT_hmi_code_data[] = { "<text f=16 vx0=285 m=0 mdl=test aux=0>100</>" };


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static curve_ctl_t *arr_p_crv[RLTHMI_NUM_BARS];
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_RT_trendHMI( HMI *self, void *arg);
static void RT_trendHmi_InitSheet( HMI *self );
static void RT_trendHmi_HideSheet( HMI *self );
static void RLT_dataVisual( HMI *self, void *arg);
static void	RT_trendHmi_Show( HMI *self);


static void	RT_trendHmi_HitHandle( HMI *self, char *s);

//焦点

static void	RLT_init_focus(HMI *self);
static void	RLT_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	RLT_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);

//命令
static void RT_trendHmi_EnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg);

static int RLT_trendHmi_MdlUpdata( Observer *self, void *p_srcMdl);


static void Bulid_rtCurveSheet( RLT_trendHMI *self);


static void RLT_Init_curve(RLT_trendHMI *self);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

RLT_trendHMI *Get_RLT_trendHMI(void)
{
	static RLT_trendHMI *singal_RTTHmi = NULL;
	if( singal_RTTHmi == NULL)
	{
		singal_RTTHmi = RLT_trendHMI_new();
		g_p_RLT_trendHmi = SUPER_PTR( singal_RTTHmi, HMI);
		
	}
	
	return singal_RTTHmi;
	
}

CTOR( RLT_trendHMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_RT_trendHMI);
FUNCTION_SETTING( HMI.initSheet, RT_trendHmi_InitSheet);
FUNCTION_SETTING( HMI.hide, RT_trendHmi_HideSheet);
FUNCTION_SETTING( HMI.show, RT_trendHmi_Show);
FUNCTION_SETTING( HMI.dataVisual, RLT_dataVisual);

FUNCTION_SETTING( HMI.init_focus, RLT_init_focus);
FUNCTION_SETTING( HMI.clear_focus, RLT_clear_focus);
FUNCTION_SETTING( HMI.show_focus, RLT_show_focus);

FUNCTION_SETTING( HMI.hitHandle, RT_trendHmi_HitHandle);



FUNCTION_SETTING( shtCmd.shtExcute, RT_trendHmi_EnterCmdHdl);
FUNCTION_SETTING( Observer.update, RLT_trendHmi_MdlUpdata);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_RT_trendHMI( HMI *self, void *arg)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	
	Curve_init();
	
	p_shtctl = GetShtctl();
	
	//初始化背景图片
	p_exp = ExpCreate( "pic");
	cthis->p_bkg = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_bkPic, cthis->p_bkg) ;
	
	
	p_exp = ExpCreate( "text");
	cthis->p_title = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_title, cthis->p_title) ;
	
	p_exp = ExpCreate( "text");
	cthis->p_div = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_div, cthis->p_div) ;
	
	Bulid_rtCurveSheet(cthis);

	//初始化焦点
//	self->init_focus(self);
	
	
	
	return RET_OK;

}



static void RT_trendHmi_InitSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	int i,  h = 0;

	Sheet_updown( cthis->p_bkg, h++);
	Sheet_updown( g_p_curve_bkg, h++);
	Sheet_updown( cthis->p_title, h++);
	Sheet_updown( cthis->p_div, h++);
	for( i = 0; i < RLTHMI_NUM_BARS; i++) {
		Sheet_updown( cthis->arr_p_sht_data[i], h++);
	}
	
	RLT_Init_curve(cthis);
	
	self->init_focus(self);
}

static void RT_trendHmi_HideSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	
	int i;
	
	cthis->flags = 0;
	
	for( i = RLTHMI_NUM_BARS - 1; i >= 0; i--) {
		Sheet_updown( cthis->arr_p_sht_data[i], -1);
	}
	Sheet_updown( cthis->p_div, -1);
	Sheet_updown( cthis->p_title, -1);
	Sheet_updown( g_p_curve_bkg, -1);
	Sheet_updown( cthis->p_bkg, -1);
	
	
//	self->clear_focus(self, 0, 0);
	
	Focus_free(self->p_fcuu);
}	



static void	RLT_init_focus(HMI *self)
{
	int	col = 0; 
	RLT_trendHMI *cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	
	self->p_fcuu = Focus_alloc(1, RLTHMI_NUM_BARS + 1);
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
	Focus_Set_sht(self->p_fcuu, 0, 0, cthis->p_div);
	for(col = 0; col < RLTHMI_NUM_BARS; col ++) {
		Focus_Set_sht(self->p_fcuu, 0, col + 1, cthis->arr_p_sht_data[col]);
	}		
	
	
}

static void	RLT_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet *p_sht = Focus_Get_sht(self->p_fcuu, fouse_row, fouse_col);
	
	if(p_sht == NULL)
		return;
	p_sht->cnt.effects = GP_CLR_EFF( p_sht->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_sht);
//	Flush_LCD();
}
static void	RLT_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet *p_sht = Focus_Get_focus(self->p_fcuu);
	
	if(p_sht == NULL)
		return;
	p_sht->cnt.effects = GP_SET_EFF( p_sht->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_sht);
//	Flush_LCD();
}

static void	RT_trendHmi_Show( HMI *self )
{
	RLT_trendHMI		*cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	g_p_curHmi = self;
	
	
	cthis->flags = 1;
	Sheet_refresh(cthis->p_bkg);
	self->dataVisual(self, NULL);
	Flush_LCD();
//	self->show_focus( self, 0, 0);
}

static void	RT_trendHmi_HitHandle( HMI *self, char *s)
{
//	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
//	shtCmd		*p_cmd;
	
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = self->p_fcuu->focus_col;
	uint8_t		chgFouse = 0;

	if( !strcmp( s, HMIKEY_UP) )
	{
		Focus_move_left(self->p_fcuu);
		chgFouse = 1;
	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		Focus_move_right(self->p_fcuu);
		chgFouse = 1;
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

	if( !strcmp( s, HMIKEY_ENTER))
	{
		self->switchHMI(self, g_p_HMI_menu);
		
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
	if( chgFouse)
	{
		
		
		
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
}



static void RT_trendHmi_EnterCmdHdl( shtCmd *self, struct SHEET *p_sht, void *arg)
{
	RLT_trendHMI	*cthis = SUB_PTR( self, shtCmd, RLT_trendHMI);
	HMI		*selfHmi = SUPER_PTR( cthis, HMI);
	HMI		*srcHmi = ( HMI *)arg;
	
	srcHmi->switchHMI( srcHmi, selfHmi);
	
}

static void RLT_dataVisual( HMI *self, void *arg)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	int					i;
	
	for(i = 0; i < RLTHMI_NUM_BARS; i++) {
		
		Curve_draw(arr_p_crv[i]);
	}

	
}

// 


static void Bulid_rtCurveSheet( RLT_trendHMI *self)
{
	
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	Model			*p_mdl = NULL;
	short 			i;
	uint8_t			data_vy[6] = {65, 98, 128, 160, 195, 225};
	
	
	p_shtctl = GetShtctl();

	for( i = 0; i < RLTHMI_NUM_BARS; i++) {
		
		
		p_exp = ExpCreate("text");
		self->arr_p_sht_data[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)RT_hmi_code_data, self->arr_p_sht_data[i]) ;
		
		self->arr_p_sht_data[i]->cnt.colour = arr_clrs[i];
		self->arr_p_sht_data[i]->area.y0 = data_vy[i];
		
	}	
	p_mdl = ModelCreate("time");
	p_mdl->attach(p_mdl, &self->Observer);
	
}


static int RLT_trendHmi_MdlUpdata( Observer *self, void *p_srcMdl)
{
	RLT_trendHMI 	*cthis = SUB_PTR( self, Observer, RLT_trendHMI);
	HMI				*p_hmi = SUPER_PTR(cthis, HMI);
	Model			*p_mdl = ModelCreate("test");
	
	int				range = 100;
	
	uint8_t			vy0 = 208;
	uint8_t			height = 150;
	uint8_t			i = 0;
	int				y = 0;
	
	//本界面不是当前显示界面，退出
	if(cthis->flags == 0)
		return RET_OK;
	//刷新时间未到就直接退出
	
	g_p_curve_bkg->p_gp->vdraw(g_p_curve_bkg->p_gp, &g_p_curve_bkg->cnt, &g_p_curve_bkg->area);
	cthis->p_title->p_gp->vdraw(cthis->p_title->p_gp, &cthis->p_title->cnt, &cthis->p_title->area);
	cthis->p_div->p_gp->vdraw(cthis->p_div->p_gp, &cthis->p_div->cnt, &cthis->p_div->area);

	
	for(i = 0; i < RLTHMI_NUM_BARS; i++) {
		y = p_mdl->getMdlData(p_mdl, AUX_PERCENTAGE, &range);
		y = vy0 - height * y / 100;
//		y = 100;
		
		cthis->arr_p_sht_data[i]->cnt.data = p_mdl->to_string(p_mdl, AUX_DATA, NULL);
		cthis->arr_p_sht_data[i]->cnt.len = strlen(cthis->arr_p_sht_data[i]->cnt.data);
		cthis->arr_p_sht_data[i]->p_gp->vdraw(cthis->arr_p_sht_data[i]->p_gp, &cthis->arr_p_sht_data[i]->cnt, &cthis->arr_p_sht_data[i]->area);
		
		Curve_add_point(arr_p_crv[i], y);
		Curve_draw(arr_p_crv[i]);
	}
	Flush_LCD();
	
//	p_hmi->show(p_hmi);
	
	
	
	return RET_OK;
	
}

static void RLT_Init_curve(RLT_trendHMI *self)
{
	curve_ctl_t *p_cctl;
	int i = 0;
	
	for(i = 0; i < RLTHMI_NUM_BARS; i++) {
		p_cctl = &g_curve[i];
		arr_p_crv[i] = p_cctl;
		
		Curve_clean(p_cctl);
		Curve_set(p_cctl, 240, arr_clrs[i], 239, 1);
	}
	
	
	
}




