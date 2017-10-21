#include "News_Power_down_HMI.h"
#include "sdhDef.h"
#include "ExpFactory.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NEWS_PWRDN_PICNUM		"16"
#define NEWS_PWRDN_TITLE		"µôµçÒ»ÀÀ"
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI 	*g_p_News_PwrDn_HMI;
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
static int	Init_News_PwrDn_HMI(HMI *self, void *arg);
static void Show_News_PwrDn_HMI(HMI *self);
static void	News_PwrDn_HMI_hide(HMI *self);
static void	PwrDn_initSheet(HMI *self);
static void	News_PwrDn_HMI_init_focus(HMI *self);

static void	News_PwrDn_HMI_hitHandle( HMI *self, char *s_key);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

News_PwrDn_HMI *Get_PwrDn_HMI(void)
{
	static News_PwrDn_HMI *singal_News_PwrDn_HMI = NULL;
	if( singal_News_PwrDn_HMI == NULL)
	{
		singal_News_PwrDn_HMI = News_PwrDn_HMI_new();
		g_p_News_PwrDn_HMI = SUPER_PTR(singal_News_PwrDn_HMI, HMI);

	}
	
	return singal_News_PwrDn_HMI;
	
}

CTOR(News_PwrDn_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_News_PwrDn_HMI);
FUNCTION_SETTING(HMI.initSheet, PwrDn_initSheet);
FUNCTION_SETTING(HMI.hide, News_PwrDn_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_News_PwrDn_HMI);

FUNCTION_SETTING(HMI.hitHandle, News_PwrDn_HMI_hitHandle);
FUNCTION_SETTING(HMI.init_focus, News_PwrDn_HMI_init_focus);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_News_PwrDn_HMI(HMI *self, void *arg)
{
//	News_PwrDn_HMI		*cthis = SUB_PTR( self, HMI, News_PwrDn_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
	
	
	
		
	return RET_OK;
}

static void Show_News_PwrDn_HMI(HMI *self)
{
//	News_PwrDn_HMI		*cthis = SUB_PTR( self, HMI, News_PwrDn_HMI);
	Sheet_refresh(g_p_sht_bkpic);
//	cthis->p_bkg->p_gp->vdraw( cthis->p_bkg->p_gp, &cthis->p_bkg->cnt, &cthis->p_bkg->area);
	self->show_focus( self,self->p_fcuu->focus_row, 0);
}
static void	PwrDn_initSheet(HMI *self)
{
//	News_PwrDn_HMI		*cthis = SUB_PTR( self, HMI, News_PwrDn_HMI);
	int  			h = 0;
	Expr 			*p_exp ;
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)news_cpic, g_p_cpic) ;
	

	g_p_sht_bkpic->cnt.data = NEWS_PWRDN_PICNUM;
	
	g_p_sht_title->cnt.data = NEWS_PWRDN_TITLE;
	g_p_sht_title->cnt.len = strlen(NEWS_PWRDN_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(g_p_ico_memu, h++);
	Sheet_updown(g_p_ico_pgup, h++);
	Sheet_updown(g_p_ico_pgdn, h++);
	Sheet_updown(g_p_ico_eraseTool, h++);

	

	self->init_focus(self);
}
static void	News_PwrDn_HMI_hide(HMI *self)
{
//	News_PwrDn_HMI		*cthis = SUB_PTR( self, HMI, News_PwrDn_HMI);

	Sheet_updown( g_p_ico_eraseTool, -1);
	Sheet_updown( g_p_ico_pgdn, -1);
	Sheet_updown( g_p_ico_pgup, -1);
	Sheet_updown( g_p_ico_memu, -1);
	Sheet_updown( g_p_shtTime, -1);
	Sheet_updown( g_p_sht_title, -1);
	Sheet_updown( g_p_sht_bkpic, -1);
	
	
	
	Focus_free(self->p_fcuu);
}


static void	News_PwrDn_HMI_init_focus(HMI *self)
{
//	News_PwrDn_HMI		*cthis = SUB_PTR( self, HMI, News_PwrDn_HMI);
	self->p_fcuu = Focus_alloc(1, 4);
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
	Focus_Set_sht(self->p_fcuu, 0, 0, g_p_ico_memu);
	Focus_Set_sht(self->p_fcuu, 0, 1, g_p_ico_pgup);
	Focus_Set_sht(self->p_fcuu, 0, 2, g_p_ico_pgdn);
	Focus_Set_sht(self->p_fcuu, 0, 3, g_p_ico_eraseTool);
		
}




static void	News_PwrDn_HMI_hitHandle(HMI *self, char *s_key)
{
	
//	News_PwrDn_HMI		*cthis = SUB_PTR( self, HMI, News_PwrDn_HMI);
	sheet		*p_focus;
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
	if( !strcmp(s_key, HMIKEY_ENTER))
	{
		p_focus = Focus_Get_focus(self->p_fcuu);
		if(p_focus == NULL)
			goto exit;
		switch(p_focus->id)
		{
			case ICO_ID_MENU:
				self->switchHMI(self, g_p_HMI_menu);
				break;
			case ICO_ID_PGUP:
				break;
			case ICO_ID_PGDN:
				break;
			case ICO_ID_ERASETOOL:
				break;
			
			default:
				break;
						
			
		}
		
		
	}
	
	if( !strcmp(s_key, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
	if( chgFouse)
	{	
		self->clear_focus(self, 0, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
	exit:
		return;
}



