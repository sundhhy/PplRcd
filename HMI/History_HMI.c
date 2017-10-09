#include "History_HMI.h"
#include "sdhDef.h"
#include "ExpFactory.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define HISTORY_PICNUM		"14"
#define HISTORY_TITLE		"ÀúÊ·Ç÷ÊÆ"

static const char history_hmi_code_cpic[] =  {"<cpic vx0=260 vy0=30 vx1=320 vy1=240>23</>" };

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI 	*g_p_History_HMI;
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
static int	Init_History_HMI(HMI *self, void *arg);
static void Show_History_HMI(HMI *self);
static void	History_HMI_hide(HMI *self);
static void	History_initSheet(HMI *self);
static void	History_HMI_init_focus(HMI *self);

static void	History_HMI_hitHandle( HMI *self, char *s_key);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

History_HMI *Get_History_HMI(void)
{
	static History_HMI *singal_History_HMI = NULL;
	if( singal_History_HMI == NULL)
	{
		singal_History_HMI = History_HMI_new();
		g_p_History_HMI = SUPER_PTR(singal_History_HMI, HMI);

	}
	
	return singal_History_HMI;
	
}

CTOR(History_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_History_HMI);
FUNCTION_SETTING(HMI.initSheet, History_initSheet);
FUNCTION_SETTING(HMI.hide, History_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_History_HMI);

FUNCTION_SETTING(HMI.hitHandle, History_HMI_hitHandle);
FUNCTION_SETTING(HMI.init_focus, History_HMI_init_focus);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_History_HMI(HMI *self, void *arg)
{
	History_HMI		*cthis = SUB_PTR( self, HMI, History_HMI);
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	short				i = 0;	
	p_shtctl = GetShtctl();
	

	p_exp = ExpCreate( "pic");
	
		
	return RET_OK;
}

static void Show_History_HMI(HMI *self)
{
	History_HMI		*cthis = SUB_PTR( self, HMI, History_HMI);
	Sheet_refresh(g_p_sht_bkpic);
//	cthis->p_bkg->p_gp->vdraw( cthis->p_bkg->p_gp, &cthis->p_bkg->cnt, &cthis->p_bkg->area);
	self->show_focus( self,self->p_fcuu->focus_row, 0);
}
static void	History_initSheet(HMI *self)
{
	History_HMI		*cthis = SUB_PTR( self, HMI, History_HMI);
	int  			i, h = 0;
	Expr 			*p_exp ;
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)history_hmi_code_cpic, g_p_cpic) ;
	

	g_p_sht_bkpic->cnt.data = HISTORY_PICNUM;

	
	g_p_sht_title->cnt.data = HISTORY_TITLE;
	g_p_sht_title->cnt.len = strlen(HISTORY_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(g_p_ico_memu, h++);
	for(i = 0; i < NUM_CHANNEL; i++) {
		Sheet_updown(g_arr_p_chnData[i], h++);
		Sheet_updown(g_arr_p_check[i], h++);
	}

	self->init_focus(self);
}
static void	History_HMI_hide(HMI *self)
{
	History_HMI		*cthis = SUB_PTR( self, HMI, History_HMI);

	
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	
	Focus_free(self->p_fcuu);
}


static void	History_HMI_init_focus(HMI *self)
{
	History_HMI		*cthis = SUB_PTR( self, HMI, History_HMI);
	self->p_fcuu = Focus_alloc(1, 4);

}




static void	History_HMI_hitHandle(HMI *self, char *s_key)
{
	
	History_HMI		*cthis = SUB_PTR( self, HMI, History_HMI);
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



