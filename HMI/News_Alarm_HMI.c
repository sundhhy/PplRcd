#include "News_Alarm_HMI.h"
#include "sdhDef.h"
#include "ExpFactory.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NEWS_ALARM_PICNUM		"16"
#define ALARM_TITLE		"±¨¾¯Ò»ÀÀ"
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI 	*g_p_News_Alarm_HMI;
sheet	*g_p_news_groundFinish;
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
static int	Init_News_Alarm_HMI(HMI *self, void *arg);
static void Show_News_Alarm_HMI(HMI *self);
static void	News_Alarm_HMI_hide(HMI *self);
static void	Alarm_initSheet(HMI *self);
static void	NWA_Build_button(HMI *self);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

News_Alarm_HMI *Get_News_Alarm_HMI(void)
{
	static News_Alarm_HMI *singal_News_Alarm_HMI = NULL;
	if( singal_News_Alarm_HMI == NULL)
	{
		singal_News_Alarm_HMI = News_Alarm_HMI_new();
		g_p_News_Alarm_HMI = SUPER_PTR(singal_News_Alarm_HMI, HMI);

	}
	
	return singal_News_Alarm_HMI;
	
}

CTOR(News_Alarm_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_News_Alarm_HMI);
FUNCTION_SETTING(HMI.initSheet, Alarm_initSheet);
FUNCTION_SETTING(HMI.hide, News_Alarm_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_News_Alarm_HMI);

FUNCTION_SETTING(HMI.hitHandle, Main_HMI_hit);
FUNCTION_SETTING(HMI.build_button, NWA_Build_button);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_News_Alarm_HMI(HMI *self, void *arg)
{
//	News_Alarm_HMI		*cthis = SUB_PTR( self, HMI, News_Alarm_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
	

	
	
		
	return RET_OK;
}

static void Show_News_Alarm_HMI(HMI *self)
{
//	News_Alarm_HMI		*cthis = SUB_PTR( self, HMI, News_Alarm_HMI);
	Sheet_refresh(g_p_sht_bkpic);
//	cthis->p_bkg->p_gp->vdraw( cthis->p_bkg->p_gp, &cthis->p_bkg->cnt, &cthis->p_bkg->area);
	self->show_focus( self,self->p_fcuu->focus_row, 0);
}
static void	Alarm_initSheet(HMI *self)
{
//	News_Alarm_HMI		*cthis = SUB_PTR( self, HMI, News_Alarm_HMI);
	
	int  			h = 0;
	Expr 			*p_exp ;
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)news_cpic, g_p_cpic) ;
	

	g_p_sht_bkpic->cnt.data = NEWS_ALARM_PICNUM;

	
	g_p_sht_title->cnt.data = ALARM_TITLE;
	g_p_sht_title->cnt.len = strlen(ALARM_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
//	Sheet_updown(g_p_ico_memu, h++);
//	Sheet_updown(g_p_ico_pgup, h++);
//	Sheet_updown(g_p_ico_pgdn, h++);
//	Sheet_updown(g_p_ico_eraseTool, h++);

	

	self->init_focus(self);
}
static void	News_Alarm_HMI_hide(HMI *self)
{
//	News_Alarm_HMI		*cthis = SUB_PTR( self, HMI, News_Alarm_HMI);

//	Sheet_updown(g_p_ico_eraseTool, -1);
//	Sheet_updown(g_p_ico_pgdn, -1);
//	Sheet_updown(g_p_ico_pgup, -1);
//	Sheet_updown(g_p_ico_memu, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	
//	Focus_free(self->p_fcuu);
}


static void NWA_btn_hdl(void *arg, uint8_t btn_id)
{
	HMI					*self	= (HMI *)arg;		
	
	switch(btn_id)
	{
		
		
		case ICO_ID_ERASETOOL:
			
			break;	
		case ICO_ID_PGUP:
			
			break;
		case ICO_ID_PGDN:
			
			break;
	
	}

	
		
}

static void	NWA_Build_button(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	
	p->build_each_btn(0, BTN_TYPE_MENU, Main_btn_hdl, self);
	p->build_each_btn(1, BTN_TYPE_PGUP, NWA_btn_hdl, self);
	p->build_each_btn(2, BTN_TYPE_PGDN, NWA_btn_hdl, self);
	p->build_each_btn(3, BTN_TYPE_ERASE, NWA_btn_hdl, self);
		
}








