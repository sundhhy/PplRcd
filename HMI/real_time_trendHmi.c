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



//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_RLT_trendHmi;

sheet  		*g_arr_p_check[NUM_CHANNEL]; 		//是否显示的指示图形
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define		RLTHMI_BKPICNUM		"14"
#define		RLTHMI_TITLE		"实时趋势"

static const char RT_hmi_code_chninfo[] =  {"<cpic vx0=260 vy0=30 vx1=320 vy1=240>23</>" };


static const char RT_hmi_code_div[] = { "<text vx0=8 vy0=36 f=16 m=0 clr=red>1</>" };

static const char RT_hmi_code_data[] = { "<text f=16 vx0=285 m=0 aux=3>100</>" };
static const char RLT_hmi_code_chnshow[] ={ "<icon vx0=265 vy0=62 xn=4 yn=1 n=0>22</>" };
#define  CHNSHOW_ROW_SPACE		32

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static curve_ctl_t *arr_p_crv[RLTHMI_NUM_CURVE];
	
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

//曲线
static int RLT_trendHmi_MdlUpdata( Observer *self, void *p_srcMdl);
static void Bulid_rtCurveSheet( RLT_trendHMI *self);
static void RLT_Init_curve(RLT_trendHMI *self);

//数据
static void RLTHmi_Init_chnSht(void);
static int RLTHmi_Data_update(void *p_data, void *p_mdl);

//键盘
static int	RLT_div_input(void *self, void *data, int len);
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
	short				i = 0;
	
	
	
	Curve_init();
	
	p_shtctl = GetShtctl();
	
	
	
	
	
	p_exp = ExpCreate( "text");
	cthis->p_div = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_div, cthis->p_div);
	cthis->str_div[0] = cthis->p_div->cnt.data[0];
	cthis->div = atoi(cthis->p_div->cnt.data);
	cthis->p_div->cnt.data = cthis->str_div;
	cthis->p_div ->p_enterCmd = &g_keyHmi->shtCmd;
	cthis->p_div->input =  RLT_div_input;
	Bulid_rtCurveSheet(cthis);
	
	
	//初始化通道勾选图形
	p_exp = ExpCreate( "pic");
	cthis->chn_show_map = 0xff;
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
		g_arr_p_check[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)RLT_hmi_code_chnshow, g_arr_p_check[i]);
		g_arr_p_check[i]->area.y0 += i * CHNSHOW_ROW_SPACE;
		g_arr_p_check[i]->area.y1 += i * CHNSHOW_ROW_SPACE;
		g_arr_p_check[i]->id = SHTID_CHECK(i);
	}

	//初始化焦点
//	self->init_focus(self);
	
	
	cthis->flags = 0;
	return RET_OK;

}



static void RT_trendHmi_InitSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	Expr 			*p_exp ;
	int i,  h = 0;
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)RT_hmi_code_chninfo, g_p_cpic) ;
	
	g_p_sht_bkpic->cnt.data = RLTHMI_BKPICNUM;

	g_p_sht_title->cnt.data = RLTHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(RLTHMI_TITLE);

	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_curve_bkg, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(g_p_ico_memu, h++);
	Sheet_updown(cthis->p_div, h++);
//	Sheet_updown( cthis->p_clean_chnifo, h++);
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
		Sheet_updown(g_arr_p_chnData[i], h++);
		Sheet_updown(g_arr_p_check[i], h++);
	}
	
	RLTHmi_Init_chnSht();
	RLT_Init_curve(cthis);
	self->init_focus(self);
}

static void RT_trendHmi_HideSheet( HMI *self )
{
	RLT_trendHMI			*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	
	int i;
	
	cthis->flags = 0;
	
	for( i = RLTHMI_NUM_CURVE - 1; i >= 0; i--) {
		Sheet_updown(g_arr_p_check[i], -1);
		Sheet_updown(g_arr_p_chnData[i], -1);
	}
//	Sheet_updown( cthis->p_clean_chnifo, -1);
	Sheet_updown(cthis->p_div, -1);
	Sheet_updown(g_p_ico_memu, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_curve_bkg, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
//	self->clear_focus(self, 0, 0);
//	self->clear_focus( self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
	Focus_free(self->p_fcuu);
}	



static void	RLT_init_focus(HMI *self)
{
	int	col = 0; 
	RLT_trendHMI *cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	
	self->p_fcuu = Focus_alloc(1, RLTHMI_NUM_CURVE + 1);
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
	Focus_Set_sht(self->p_fcuu, 0, 0, cthis->p_div);
	for(col = 0; col < RLTHMI_NUM_CURVE; col ++) {
		Focus_Set_sht(self->p_fcuu, 0, col + 1, g_arr_p_check[col]);
	}		
	
	
}

static void	RLT_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet *p_sht = Focus_Get_sht(self->p_fcuu, fouse_row, fouse_col);
	
	if(p_sht == NULL)
		return;
	if(IS_CHECK(p_sht->id)) {
		//这是更具勾选图标在图片中的位置来编码的
		if(p_sht->area.n == 1)
			p_sht->area.n = 0;
		else if(p_sht->area.n == 3)
			p_sht->area.n = 2;
	} else {
		p_sht->cnt.effects = GP_CLR_EFF( p_sht->cnt.effects, EFF_FOCUS);
	}
	Sheet_slide(p_sht);
}
static void	RLT_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	sheet *p_sht = Focus_Get_focus(self->p_fcuu);
	
	if(p_sht == NULL)
		return;
	
	if(IS_CHECK(p_sht->id)) {
		//这是更具勾选图标在图片中的位置来编码的
		if(p_sht->area.n == 0)
			p_sht->area.n = 1;
		else if(p_sht->area.n == 2)
			p_sht->area.n = 3;
	} else {
		p_sht->cnt.effects = GP_SET_EFF( p_sht->cnt.effects, EFF_FOCUS);
	}
	Sheet_slide( p_sht);
}
static int	RLT_div_input(void *self, void *data, int len)
{
	RLT_trendHMI		*cthis = Get_RLT_trendHMI();
	if(len > 2)
		return ERR_PARAM_BAD;
	cthis->str_div[0] = ((char *)data)[0];
	cthis->str_div[1] = ((char *)data)[1];
	cthis->p_div->cnt.len = len;
	cthis->div = atoi(data);
	
	return RET_OK;
}
static void	RT_trendHmi_Show( HMI *self )
{
	RLT_trendHMI		*cthis = SUB_PTR(self, HMI, RLT_trendHMI);
	g_p_curHmi = self;
	
	
	
	Sheet_refresh(g_p_sht_bkpic);
	self->dataVisual(self, NULL);
	self->show_focus( self, 0, 0);
	Flush_LCD();
	cthis->flags = 1;
//	
}

static void	RT_trendHmi_HitHandle( HMI *self, char *s)
{
	RLT_trendHMI		*cthis = SUB_PTR( self, HMI, RLT_trendHMI);
	shtCmd		*p_cmd;
	sheet		*p_focus;
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = self->p_fcuu->focus_col;
	uint8_t		chgFouse = 0;
	uint8_t		chn = 0;

	
	cthis->flags |= 2;
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
		p_focus = Focus_Get_focus(self->p_fcuu);
		if(p_focus && IS_CHECK(p_focus->id)) {
			chn = GET_CHN_FROM_ID(p_focus->id);
			
			if(cthis->chn_show_map & (1 << chn)) {
				cthis->chn_show_map &= ~(1 << chn);
				p_focus->area.n = 2;
				//清除数字显示
				g_arr_p_chnData[chn]->cnt.data = "   ";
				g_arr_p_chnData[chn]->cnt.len = 3;
				g_arr_p_chnData[chn]->p_gp->vdraw(g_arr_p_chnData[chn]->p_gp,\
				&g_arr_p_chnData[chn]->cnt, &g_arr_p_chnData[chn]->area);
			} else {
				p_focus->area.n = 0;
				cthis->chn_show_map |= 1 << chn;
			}
			p_focus->p_gp->vdraw(p_focus->p_gp, &p_focus->cnt, &p_focus->area);
			Flush_LCD();
			
		} else {
			
			
			if(p_focus) {
				p_cmd = p_focus->p_enterCmd;
				if(p_cmd)
					p_cmd->shtExcute( p_cmd, p_focus, self);
			} else
				self->switchHMI(self, g_p_HMI_menu);
		}
	}
	
	
	if( chgFouse)
	{
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
	}
	

	
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
	cthis->flags &= ~2;
	
	
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
	
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
		
		Curve_draw(arr_p_crv[i]);
	}

	
}

// 


static void Bulid_rtCurveSheet( RLT_trendHMI *self)
{
	
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	Model			*p_mdl = NULL;
//	short 			i;
	uint8_t			data_vy[6] = {65, 98, 128, 160, 195, 225};
	
	
	p_shtctl = GetShtctl();

//	for( i = 0; i < RLTHMI_NUM_CURVE; i++) {
//		
//		
//		p_exp = ExpCreate("text");
//		self->arr_p_sht_data[i] = Sheet_alloc( p_shtctl);
//		p_exp->inptSht( p_exp, (void *)RT_hmi_code_data, self->arr_p_sht_data[i]) ;
//		
//		self->arr_p_sht_data[i]->cnt.colour = arr_clrs[i];
//		self->arr_p_sht_data[i]->area.y0 = data_vy[i];
//		
//	}	
	p_mdl = ModelCreate("time");
	p_mdl->attach(p_mdl, &self->Observer);
	
}


static int RLT_trendHmi_MdlUpdata( Observer *self, void *p_srcMdl)
{
	RLT_trendHMI 	*cthis = SUB_PTR( self, Observer, RLT_trendHMI);
	HMI				*p_hmi = SUPER_PTR(cthis, HMI);
	Model			*p_mdl ;
	
	int				range = 100;
	
	uint8_t			vy0 = 208;
	uint8_t			height = 150;
	uint8_t			i = 0;
	int				y = 0;
	
	//本界面不是当前显示界面，退出
	if(cthis->flags == 0)
		return RET_OK;
	//按键处理中，退出
	if(cthis->flags & 2)
		return RET_OK;
	
	cthis->count ++;
	if(cthis->count < cthis->div)
		return RET_OK;
	cthis->count = 0;
	//刷新时间未到就直接退出
	
	g_p_curve_bkg->p_gp->vdraw(g_p_curve_bkg->p_gp, &g_p_curve_bkg->cnt, &g_p_curve_bkg->area);
//	cthis->p_title->p_gp->vdraw(cthis->p_title->p_gp, &cthis->p_title->cnt, &cthis->p_title->area);
//	cthis->p_div->p_gp->vdraw(cthis->p_div->p_gp, &cthis->p_div->cnt, &cthis->p_div->area);

	
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {

		if((cthis->chn_show_map & (1 << i)) == 0) {
			continue;
		}
		
		p_mdl = g_arr_p_chnData[i]->p_mdl;
		y = p_mdl->getMdlData(p_mdl, g_arr_p_chnData[i]->cnt.mdl_aux, &range);
		y = vy0 - height * y / 100;
		
		if(y < (vy0 - height))
			y = vy0 - height;
//		y = 100;
		
		g_arr_p_chnData[i]->cnt.data = p_mdl->to_string(p_mdl, AUX_DATA, NULL);
		g_arr_p_chnData[i]->cnt.len = strlen(g_arr_p_chnData[i]->cnt.data);
		g_arr_p_chnData[i]->p_gp->vdraw(g_arr_p_chnData[i]->p_gp, &g_arr_p_chnData[i]->cnt, &g_arr_p_chnData[i]->area);
		
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
	
	for(i = 0; i < RLTHMI_NUM_CURVE; i++) {
		p_cctl = &g_curve[i];
		arr_p_crv[i] = p_cctl;
		
		Curve_clean(p_cctl);
		self->count = 0;
		Curve_set(p_cctl, 240/self->div, arr_clrs[i], 239, self->div);
	}
	
	
	
}

static void RLTHmi_Init_chnSht(void)
{
	Expr 		*p_exp ;
	Model		*p_mdl = NULL;
	uint8_t			data_vy[6] = {65, 98, 128, 160, 195, 225};
	uint16_t			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		p_exp->inptSht( p_exp, (void *)RT_hmi_code_data, g_arr_p_chnData[i]) ;
		
		
		
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->cnt.data = prn_buf[i];
		
		g_arr_p_chnData[i]->update = RLTHmi_Data_update;

		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->area.y0 = data_vy[i];
	}
}

static int RLTHmi_Data_update(void *p_data, void *p_mdl)
{
	
	
	return 0;
	
	
	
}





