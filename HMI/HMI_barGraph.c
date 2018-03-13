#include "HMI_barGraph.h"
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

#define BARHMI_NUM_BTNROW		1
#define BARHMI_NUM_BTNCOL		4

#define BARHMI_BKPICNUM		"12"
#define BARHMI_TITLE		"棒图画面"

static const char barhmi_code_clean[] = { "<cpic vx0=0 vy0=0 >12</>" };
static const char barhmi_code_bar[] = { "<box bx=35 ></>" };
static const char barhmi_code_textPrcn[] = { "<text f=16 m=0 mdl=chn_0 aux=3>100</>" };



//static const hmiAtt_t	barHmiAtt = { 4,4, COLOUR_GRAY, BARHMI_NUM_BTNROW, BARHMI_NUM_BTNCOL};


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


	
//static sheet  *arr_p_sht_select[BARHMI_NUM_BTNCOL];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_barGhHMI( HMI *self, void *arg);
static void BarHmi_InitSheet( HMI *self );
static void BarHmi_HideSheet( HMI *self );

static void	BarHmi_Show( HMI *self);


//static void	BarHmi_HitHandle( HMI *self, char *s);


//柱形图操作函数
static void BarHmi_Init_chnSht(void);
static void Init_bar( barGhHMI *self);
static int BarHmi_Data_update(void *p_data, void *p_mdl);
static int BarHmi_Util_update(void *p_data, void *p_mdl);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

barGhHMI *Get_barGhHMI(void)
{
	static barGhHMI *singal_barHmi = NULL;
	if( singal_barHmi == NULL)
	{
		singal_barHmi = barGhHMI_new();
		if(singal_barHmi  == NULL) while(1);
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

FUNCTION_SETTING( HMI.hitHandle, Main_HMI_hit);
FUNCTION_SETTING(HMI.build_component, Main_HMI_build_button);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_barGhHMI( HMI *self, void *arg)
{
	barGhHMI		*cthis = SUB_PTR( self, HMI, barGhHMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
	
//	p_shtctl = GetShtctl();
	
	//初始化背景图片
//	p_exp = ExpCreate( "pic");
//	cthis->p_bar_clean = Sheet_alloc( p_shtctl);
//	p_exp->inptSht( p_exp, (void *)barhmi_code_clean, cthis->p_bar_clean) ;
	
	
	
	//初始化柱形图
	Init_bar(cthis);
	
	self->flag = 0;

	
	
	
	return RET_OK;

}



static void BarHmi_InitSheet( HMI *self )
{
	barGhHMI			*cthis = SUB_PTR( self, HMI, barGhHMI);
	int i,  h = 0;
	Expr 			*p_exp ;
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)barhmi_code_clean, g_p_cpic) ;
	

	


	g_p_sht_bkpic->cnt.data = BARHMI_BKPICNUM;

	g_p_sht_title->cnt.data = BARHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(BARHMI_TITLE);
	
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	for(i = 0; i < BARHMI_NUM_BARS; i++) {
//		Sheet_updown(cthis->arr_p_barshts[i], h++);
//		Sheet_updown(g_arr_p_chnData[i], h++);
//		Sheet_updown(g_arr_p_chnUtil[i], h++);

		
		cthis->arr_bar_height[i] = 0xffff;
	}
	
//	Sheet_updown(g_p_ico_memu, h++);
//	Sheet_updown(g_p_ico_bar, h++);
//	Sheet_updown(g_p_ico_digital, h++);
//	Sheet_updown(g_p_ico_trend, h++);
	
	BarHmi_Init_chnSht();
	//初始化焦点
	self->init_focus(self);
}

static void BarHmi_HideSheet( HMI *self )
{
//	barGhHMI			*cthis = SUB_PTR( self, HMI, barGhHMI);
	
	int i;
	
//	self->clear_focus(self, cthis->focusRow, cthis->focusCol);
	
	
//	Sheet_updown(g_p_ico_trend, -1);
//	Sheet_updown(g_p_ico_digital, -1);
//	Sheet_updown(g_p_ico_bar, -1);
//	Sheet_updown(g_p_ico_memu, -1);
	
	for( i = BARHMI_NUM_BARS - 1; i >= 0; i--) {
//		Sheet_updown( cthis->pp_bar_unit[i], -1);
//		Sheet_updown( cthis->arr_p_sht_textPrcn[i], -1);
//		Sheet_updown(g_arr_p_chnUtil[i], -1);
//		Sheet_updown(g_arr_p_chnData[i], -1);
//		Sheet_updown( cthis->arr_p_barshts[i], -1);
	}
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	self->clear_focus( self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
	
}	





static void	BarHmi_Show( HMI *self )
{
	barGhHMI		*cthis = SUB_PTR( self, HMI, barGhHMI);
	int 			i;
	g_p_curHmi = self;
	
	//刷新了背景，就要重新开始绘制
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		
		cthis->arr_bar_height[i] = 0xffff;
	}
	
//	Cal_bar( cthis);
	
	Sheet_refresh(g_p_sht_bkpic);
//	self->show_focus( self, 0, 0);
}

//static void	BarHmi_HitHandle( HMI *self, char *s)
//{
//	Set_flag_keyhandle(&self->flag, 1);
//	if( !strcmp( s, HMIKEY_UP) )
//	{

//	}
//	else if( !strcmp( s, HMIKEY_DOWN) )
//	{
//		
//	}
//	else if( !strcmp( s, HMIKEY_LEFT))
//	{
//		self->btn_backward(self);

//	}
//	else if( !strcmp( s, HMIKEY_RIGHT))
//	{

//		self->btn_forward(self);
//	}
//	
//	
//	
//	if( !strcmp( s, HMIKEY_ENTER))
//	{
//		
//		self->btn_hit(self);
//	}
//	
//	if( !strcmp( s, HMIKEY_ESC))
//	{
//		self->switchBack(self);
//	}
//	
//	Set_flag_keyhandle(&self->flag, 0);
//	
//}


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

//static void BarHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
//	sheet  			*p_fouse = arr_p_sht_select[fouse_col];
//	
//	p_fouse->cnt.effects = GP_CLR_EFF( p_fouse->cnt.effects, EFF_FOCUS);
//	Sheet_slide( p_fouse);
////	p_fouse->p_gp->vdraw ( p_fouse->p_gp, &p_fouse->cnt, &p_fouse->area);
//}
//static void BarHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
//	sheet  			*p_fouse = arr_p_sht_select[fouse_col];
//	
//	p_fouse->cnt.effects = GP_SET_EFF( p_fouse->cnt.effects, EFF_FOCUS);
//	Sheet_slide( p_fouse);
////	p_fouse->p_gp->vdraw ( p_fouse->p_gp, &p_fouse->cnt, &p_fouse->area);
//}




// 

static void Init_bar( barGhHMI *self)
{
	
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	char 			i;
//	char			chn_name[7];
	
	p_shtctl = GetShtctl();

	for( i = 0; i < BARHMI_NUM_BARS; i++) {
		p_exp = ExpCreate("box");
		self->arr_p_barshts[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)barhmi_code_bar, self->arr_p_barshts[i]) ;
		
//		p_exp = ExpCreate("text");
//		self->arr_p_sht_textPrcn[i] = Sheet_alloc( p_shtctl);
//		p_exp->inptSht( p_exp, (void *)barhmi_code_textPrcn, self->arr_p_sht_textPrcn[i]) ;
		
		
		
//		sprintf(chn_name,"chn_%d", i);
//		self->arr_p_sht_textPrcn[i]->p_mdl = ModelCreate(chn_name);
//		self->pp_bar_unit[i]->p_mdl = ModelCreate(chn_name);
//		
		
		
		self->arr_p_barshts[i]->cnt.bkc = arr_clrs[i];
		self->arr_p_barshts[i]->cnt.colour = arr_clrs[i];
		
//		self->arr_p_sht_textPrcn[i]->cnt.colour = arr_clrs[i];
//		
//		self->pp_bar_unit[i]->cnt.colour = arr_clrs[i];
		
	}
	
	
//	Cal_bar( self);
}

//竖向柱形图在屏幕上的位置计算，注意数据都是从特定的背景图片上测量得到的
//static void Cal_bar( barGhHMI *self)
//{
//	
//	uint16_t bar_vx0[BARHMI_NUM_BARS] = { 30, 78, 126, 172, 220, 268};
//	
//	uint16_t bar_vy1= 187;
//	uint16_t max_height= 116;
//	uint16_t text_vy0 = 30;
//	uint16_t unit_vy0 = 42;
//	
//	
//	uint32_t 	i = 0, j= 1000;
//	uint32_t	prcn = 0;
//	uint32_t	height = 0;
//	
//	
//	for( i = 0; i < BARHMI_NUM_BARS; i++)
//	{
//		
//		prcn = self->arr_p_sht_textPrcn[i]->p_mdl->getMdlData( self->arr_p_sht_textPrcn[i]->p_mdl, \
//			self->arr_p_sht_textPrcn[i]->cnt.mdl_aux,  &j);
//		height = max_height * prcn / 1000;
//		
//		self->arr_p_barshts[i]->area.x0 = bar_vx0[i];
//		self->arr_p_barshts[i]->area.x1 = bar_vx0[i] + self->arr_p_barshts[i]->bxsize;
//		self->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
//		self->arr_p_barshts[i]->area.y1 = bar_vy1;
//		
//		 
//		self->arr_p_sht_textPrcn[i]->p_mdl->to_string( self->arr_p_sht_textPrcn[i]->p_mdl, 0, self->arr_p_sht_textPrcn[i]->cnt.data);
//		self->arr_p_sht_textPrcn[i]->cnt.len = strlen( self->arr_p_sht_textPrcn[i]->cnt.data);
//		self->arr_p_sht_textPrcn[i]->area.x0 = bar_vx0[i];
//		self->arr_p_sht_textPrcn[i]->area.y0 = text_vy0;
//		
//		self->pp_bar_unit[i]->cnt.data = \
//			self->pp_bar_unit[i]->p_mdl->to_string( self->pp_bar_unit[i]->p_mdl, 1, NULL);
//		self->pp_bar_unit[i]->cnt.len = strlen( self->pp_bar_unit[i]->cnt.data);
//		self->pp_bar_unit[i]->area.x0 = bar_vx0[i];
//		self->pp_bar_unit[i]->area.y0 = unit_vy0;
//		
//		
//	}
//	
//	
//}

static void BarHmi_Init_chnSht(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	int			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		p_exp->inptSht( p_exp, (void *)barhmi_code_textPrcn, g_arr_p_chnData[i]) ;
		
		
		
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->cnt.data = prn_buf[i];
		
		
		
		g_arr_p_chnData[i]->update = BarHmi_Data_update;
		g_arr_p_chnUtil[i]->update = BarHmi_Util_update;

		//这是为了初始化的时候，就能让数据得到正确的坐标
//		g_arr_p_chnData[i]->update(g_arr_p_chnData[i], NULL);
//		g_arr_p_chnUtil[i]->update(g_arr_p_chnUtil[i], NULL);
	
	}
}



static int BarHmi_Data_update(void *p_data, void *p_mdl)
{
	
	barGhHMI *chis = Get_barGhHMI();
	sheet	*p_sht = (sheet *)p_data;

	uint16_t bar_vx0[BARHMI_NUM_BARS] = { 30, 78, 126, 172, 220, 268};
	
	uint16_t bar_vy1= 187;
	uint16_t max_height= 116;
	uint16_t text_vy0 = 32;
	uint16_t i = p_sht->id;

	
	uint16_t	height = 0;
	uint8_t		prcn = 0;

	



	if(IS_HMI_KEYHANDLE(g_p_barGhHmi->flag))
		return 0;	
	
//	if(Sheet_is_hide(p_sht))
//		return 0;
	if(IS_HMI_HIDE(g_p_barGhHmi->flag))
		return 0;
	
	
	p_sht->p_mdl->getMdlData( p_sht->p_mdl, AUX_PERCENTAGE,  &prcn);
	height = max_height * prcn / 100;
	
	//	p_sht->p_mdl->to_string(p_sht->p_mdl, p_sht->cnt.mdl_aux, p_sht->cnt.data);
	sprintf(p_sht->cnt.data, "%%%3d", prcn);
	p_sht->cnt.len = strlen(p_sht->cnt.data);
	p_sht->area.x0 = bar_vx0[i];
	p_sht->area.y0 = text_vy0;
	Sheet_force_slide(p_sht);
	
	//180213 优化棒图的显示，根据当前高度与上一次高度来计算本次需要更改的部分
	if(chis->arr_bar_height[i] == height)
		return 0;
	
	if(chis->arr_bar_height[i] > max_height)
	{
		//第一次显示
		chis->arr_p_barshts[i]->area.x0 = bar_vx0[i];
		chis->arr_p_barshts[i]->area.x1 = bar_vx0[i] + chis->arr_p_barshts[i]->bxsize;
		
		chis->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
		chis->arr_p_barshts[i]->area.y1 = bar_vy1;
		
		
		Sheet_force_slide( chis->arr_p_barshts[i]);
	}
	else if(chis->arr_bar_height[i] > height)
	{
		//棒图变短
		g_p_cpic->area.x0 = bar_vx0[i];
		g_p_cpic->area.x1 = bar_vx0[i] + chis->arr_p_barshts[i]->bxsize + 5;
		
		
//		g_p_cpic->area.y0 = bar_vy1 - max_height;
//		g_p_cpic->area.y1 = bar_vy1;
		g_p_cpic->area.y0 = bar_vy1 - chis->arr_bar_height[i];
		g_p_cpic->area.y1 = bar_vy1 - height;

		
		g_p_cpic->area.offset_x = 0;
		g_p_cpic->area.offset_y = 0;
		g_p_cpic->p_gp->vdraw(g_p_cpic->p_gp, &g_p_cpic->cnt, &g_p_cpic->area);
		
	}
	else
	{
		//棒图变长
		chis->arr_p_barshts[i]->area.x0 = bar_vx0[i];
		chis->arr_p_barshts[i]->area.x1 = bar_vx0[i] + chis->arr_p_barshts[i]->bxsize;
		
//		chis->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
//		chis->arr_p_barshts[i]->area.y1 = bar_vy1;
		chis->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
		chis->arr_p_barshts[i]->area.y1 = bar_vy1 - chis->arr_bar_height[i];
		
		Sheet_force_slide( chis->arr_p_barshts[i]);
	}
	chis->arr_bar_height[i] = height;
	return 0;	
}

static int BarHmi_Util_update(void *p_data, void *p_mdl)
{
	sheet		*p_sht = (sheet *)p_data;
	barGhHMI *self = Get_barGhHMI();
	uint16_t bar_vx0[BARHMI_NUM_BARS] = { 30, 78, 126, 172, 220, 268};
	
	uint16_t utit_vy0 = 46;
	uint16_t i = p_sht->id;
	
	
	if(IS_HMI_HIDE(g_p_barGhHmi->flag))
		return 0;
	if(IS_HMI_KEYHANDLE(g_p_barGhHmi->flag))
		return 0;
		
	p_sht->cnt.data = p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
	p_sht->cnt.len = strlen(p_sht->cnt.data);
	p_sht->area.x0 = bar_vx0[i];
	p_sht->area.y0 = utit_vy0;
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	
	
	Sheet_force_slide( p_sht);
	return 0;
	
}

