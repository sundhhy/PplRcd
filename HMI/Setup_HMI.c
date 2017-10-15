#include "Setup_HMI.h"
#include "sdhDef.h"
#include "ExpFactory.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define SETUP_PICNUM		"17"
#define SETUP_TITLE		"…Ë÷√"

static const char setup_hmi_code_clean[] =  {"<cpic  bx=160 by=40>17</>" };

static const char setup_hmi_code_cpic[] =  {"<icon bx=160 by=40 xn=2 yn=4 n=0>18</>" };
static const char setup_hmi_code_passwd[] =  {"<text vx0=100 vy0=44 f=24 clr=blue m=0>** ** **</>" };

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI 	*g_p_Setup_HMI;
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

 
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_Setup_HMI(HMI *self, void *arg);
static void Show_Setup_HMI(HMI *self);
static void	Setup_HMI_hide(HMI *self);
static void	Setup_initSheet(HMI *self);
static void	Setup_HMI_init_focus(HMI *self);
static void	Setup_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Setup_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Setup_HMI_hitHandle( HMI *self, char *s_key);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Setup_HMI *Get_Setup_HMI(void)
{
	static Setup_HMI *singal_Setup_HMI = NULL;
	if( singal_Setup_HMI == NULL)
	{
		singal_Setup_HMI = Setup_HMI_new();
		g_p_Setup_HMI = SUPER_PTR(singal_Setup_HMI, HMI);

	}
	
	return singal_Setup_HMI;
	
}

CTOR(Setup_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Setup_HMI);
FUNCTION_SETTING(HMI.initSheet, Setup_initSheet);
FUNCTION_SETTING(HMI.hide, Setup_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Setup_HMI);

FUNCTION_SETTING(HMI.hitHandle, Setup_HMI_hitHandle);
FUNCTION_SETTING(HMI.init_focus, Setup_HMI_init_focus);
FUNCTION_SETTING(HMI.clear_focus, Setup_HMI_clear_focus);
FUNCTION_SETTING(HMI.show_focus, Setup_HMI_show_focus);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_Setup_HMI(HMI *self, void *arg)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	short				i = 0;	
	p_shtctl = GetShtctl();
	

	p_exp = ExpCreate( "pic");
	
		
	return RET_OK;
}

static void Show_Setup_HMI(HMI *self)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	Sheet_refresh(g_p_sht_bkpic);
}
static void	Setup_initSheet(HMI *self)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	int  			i, h = 0;
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	p_shtctl = GetShtctl();
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_cpic, g_p_cpic) ;
	
	cthis->p_clean_focus = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_clean, cthis->p_clean_focus) ;
	
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_passwd, g_p_text) ;
	g_p_text ->p_enterCmd = &g_keyHmi->shtCmd;

	g_p_sht_bkpic->cnt.data = SETUP_PICNUM;

	
	g_p_sht_title->cnt.data = SETUP_TITLE;
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(g_p_text, h++);

	self->init_focus(self);
}
static void	Setup_HMI_hide(HMI *self)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	
	Sheet_free(cthis->p_clean_focus);
	
	Sheet_updown(g_p_text, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	
	Focus_free(self->p_fcuu);
}


static void	Setup_HMI_init_focus(HMI *self)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	short i;
	self->p_fcuu = Focus_alloc(5, 2);
	
	Focus_Set_sht(self->p_fcuu, 0, 0, g_p_text);
	Focus_Set_sht(self->p_fcuu, 0, 1, g_p_text);
	
	for(i = 1; i < 5; i++) {
		
		Focus_Set_sht(self->p_fcuu, i, 0, g_p_sht_bkpic);
		Focus_Set_sht(self->p_fcuu, i, 1, g_p_sht_bkpic);
	}
	
	Focus_Set_focus(self->p_fcuu, 5, 0);
}

static void	Setup_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	uint8_t			vx0 = 0;
	uint8_t			vy0 = 80;
	
	if(fouse_row == 0) {
		g_p_text->cnt.effects = GP_CLR_EFF(g_p_text->cnt.effects, EFF_FOCUS);
		Sheet_slide(g_p_text);
	} else if(fouse_row > 4 || fouse_col >1) {
		return;
	} else {
		cthis->p_clean_focus->area.x0 = vx0 + fouse_col * cthis->p_clean_focus->bxsize;
		cthis->p_clean_focus->area.y0 = vy0 + (fouse_row - 1) *  cthis->p_clean_focus->bysize;
		
		cthis->p_clean_focus->area.x1 = cthis->p_clean_focus->area.x0 + cthis->p_clean_focus->bxsize;
		cthis->p_clean_focus->area.y1 = cthis->p_clean_focus->area.y0 + cthis->p_clean_focus->bysize;
		cthis->p_clean_focus->p_gp->vdraw( cthis->p_clean_focus->p_gp, &cthis->p_clean_focus->cnt, &cthis->p_clean_focus->area);
		Flush_LCD();
	}

}
static void	Setup_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	uint8_t			row, col;
	uint8_t			vx0 = 0;
	uint8_t			vy0 = 80;

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
	

}


static void	Setup_HMI_hitHandle(HMI *self, char *s_key)
{
	
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	sheet		*p_focus;
	shtCmd		*p_cmd;
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
		if(p_focus->id == SHEET_G_TEXT) {
			p_cmd = p_focus->p_enterCmd;
			p_cmd->shtExcute(p_cmd, p_focus, self);
		} else {
			if(p_focus == NULL) 
				goto exit;
			if(self->p_fcuu->focus_row == 4 && self->p_fcuu->focus_col) {
				self->switchHMI(self, g_p_HMI_menu);
			} else {
				g_p_Setting_HMI->arg[0] = self->p_fcuu->focus_row - 1;
				g_p_Setting_HMI->arg[1] = self->p_fcuu->focus_col;
				self->switchHMI(self, g_p_Setting_HMI);
			}
		}
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



