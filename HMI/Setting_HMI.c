#include "Setting_HMI.h"
#include "sdhDef.h"
#include "ExpFactory.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define SETTING_PICNUM		"16"
#define SETTING_TITLE		"设置"

static const char setting_hmi_code_clean[] =  {"<cpic  bx=160 by=40>16</>" };
static const char setting_hmi_code_cpic[] =  {"<icon bx=160 by=40 xn=2 yn=4 n=0>18</>" };
static const char setting_hmi_code_text[] =  {"<text  f=16 clr=white m=0> </>" };


static char *setting_titles[4][2] = {{"系统设置", "通道设置"},{"报警设置", "算法设置"},\
{"显示设置", "数据备份"},{"数据打印", "退出"}};
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI 	*g_p_Setting_HMI;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 static strategy_t	*arr_p_setting_strategy[4][2] = {{&g_sys_strategy, &g_chn_strategy}, {&g_alarm_strategy, &g_art_strategy}, \
 {&g_view_strategy, &g_DBU_strategy},{&g_dataPrint_strategy, NULL}};
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_Setting_HMI(HMI *self, void *arg);
static void Show_Setting_HMI(HMI *self);
static void	Setting_HMI_hide(HMI *self);
static void	Setting_initSheet(HMI *self);
static void	Setting_HMI_init_focus(HMI *self);
static void	Setting_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Setting_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Setting_HMI_hitHandle( HMI *self, char *s_key);

static void	Show_entry(HMI *self, strategy_t *p_st);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Setting_HMI *Get_Setting_HMI(void)
{
	static Setting_HMI *singal_Setting_HMI = NULL;
	if( singal_Setting_HMI == NULL)
	{
		singal_Setting_HMI = Setting_HMI_new();
		g_p_Setting_HMI = SUPER_PTR(singal_Setting_HMI, HMI);

	}
	
	return singal_Setting_HMI;
	
}

CTOR(Setting_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Setting_HMI);
FUNCTION_SETTING(HMI.initSheet, Setting_initSheet);
FUNCTION_SETTING(HMI.hide, Setting_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Setting_HMI);

FUNCTION_SETTING(HMI.hitHandle, Setting_HMI_hitHandle);
FUNCTION_SETTING(HMI.init_focus, Setting_HMI_init_focus);
FUNCTION_SETTING(HMI.clear_focus, Setting_HMI_clear_focus);
FUNCTION_SETTING(HMI.show_focus, Setting_HMI_show_focus);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_Setting_HMI(HMI *self, void *arg)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	short				i = 0;	
	p_shtctl = GetShtctl();
	

	p_exp = ExpCreate( "pic");
	
		
	return RET_OK;
}

static void Show_Setting_HMI(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	Sheet_refresh(g_p_sht_bkpic);
	Show_entry(self, arr_p_setting_strategy[self->arg[0]][self->arg[1]]);
}
static void	Setting_initSheet(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	int  			i, h = 0;
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	p_shtctl = GetShtctl();
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_cpic, g_p_cpic) ;
	
	p_exp = ExpCreate( "text");
	cthis->p_sht_text = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_text, cthis->p_sht_text) ;
	


	g_p_sht_bkpic->cnt.data = SETTING_PICNUM;

	
	g_p_sht_title->cnt.data = setting_titles[self->arg[0]][self->arg[1]];
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);

	self->init_focus(self);
}
static void	Setting_HMI_hide(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);

	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	Sheet_free(cthis->p_sht_text);
	Focus_free(self->p_fcuu);
}


static void	Setting_HMI_init_focus(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	self->p_fcuu = Focus_alloc(5, 2);		
	Focus_Set_focus(self->p_fcuu, 0, 0);
}

static void	Setting_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	uint8_t			vx0 = 0;
	uint8_t			vy0 = 80;
	
	if(fouse_row == 0) {
		g_p_text->cnt.effects = GP_CLR_EFF(g_p_text->cnt.effects, EFF_FOCUS);
		Sheet_slide(g_p_text);
	} else if(fouse_row > 4 || fouse_col >1) {
		return;
	} else {
		
	}

}
static void	Setting_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	uint8_t			row, col;
	uint8_t			vx0 = 0;
	uint8_t			vy0 = 80;
//	if(self->p_fcuu->focus_row > 2)
//		return;
	row = self->p_fcuu->focus_row;
	col = self->p_fcuu->focus_col;
	if(row == 0) {
		g_p_text->cnt.effects = GP_SET_EFF(g_p_text->cnt.effects, EFF_FOCUS);
		Sheet_slide(g_p_text);
	} else if(row > 4 || col >1) {
		return;
	} else {
		g_p_cpic->area.n = (row - 1) * 2 + col;
		g_p_cpic->area.x0 = vx0 + col * g_p_cpic->bxsize;
		g_p_cpic->area.y0 = vy0 + (row - 1) * g_p_cpic->bysize;
		g_p_cpic->p_gp->vdraw( g_p_cpic->p_gp, &g_p_cpic->cnt, &g_p_cpic->area);
		Flush_LCD();
	}
	
//	
//	g_p_cpic->area.y0 = vy0[self->p_fcuu->focus_row];
////	g_p_cpic->area.y1 = g_p_cpic->area.y0 + g_p_cpic->bysize;
////	g_p_cpic->area.x1 = g_p_cpic->area.x0 + g_p_cpic->bxsize;
//	
//	g_p_cpic->p_gp->vdraw( g_p_cpic->p_gp, &g_p_cpic->cnt, &g_p_cpic->area);
//	Flush_LCD();
}


static void	Setting_HMI_hitHandle(HMI *self, char *s_key)
{
	
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	sheet		*p_focus;
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = self->p_fcuu->focus_col;
	uint8_t		chgFouse = 0;

	if( !strcmp( s_key, HMIKEY_LEFT) )
	{
		Focus_move_left(self->p_fcuu);
		chgFouse = 1;
	}
	
	if( !strcmp( s_key, HMIKEY_RIGHT) )
	{
		Focus_move_right(self->p_fcuu);
		chgFouse = 1;
	}
	if( !strcmp( s_key, HMIKEY_UP) )
	{
		Focus_move_up(self->p_fcuu);
		chgFouse = 1;
	}
	
	if( !strcmp( s_key, HMIKEY_DOWN) )
	{
		Focus_move_down(self->p_fcuu);
		chgFouse = 1;
	}
	if( !strcmp(s_key, HMIKEY_ENTER))
	{
		p_focus = Focus_Get_focus(self->p_fcuu);
		
		
		
	}
	
	if( !strcmp(s_key, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
	if( chgFouse)
	{	
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
	exit:
		return;
}


static void	Show_entry(HMI *self, strategy_t *p_st)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	uint8_t	row = 0;
	uint8_t	text_len = 0;
	
	if(p_st == NULL)
		return;
	
	for(row = 0; row < STRIPE_MAX_ROWS; row ++) {
		
		text_len = p_st->entry_txt(row, 0, &cthis->p_sht_text->cnt.data);
		if(text_len == 0)
			break;
		cthis->p_sht_text->cnt.len = text_len;
		
		cthis->p_sht_text->area.x0 = 0;
		if(row == 0)
			cthis->p_sht_text->area.y0 = STRIPE_VY0 ;
		else 
			cthis->p_sht_text->area.y0 = STRIPE_VY1 + (row - 1)* STRIPE_SIZE_Y;
		
		cthis->p_sht_text->p_gp->vdraw(cthis->p_sht_text->p_gp, &cthis->p_sht_text->cnt, &cthis->p_sht_text->area);
		
	}
}


