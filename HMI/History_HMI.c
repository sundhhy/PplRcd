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
static void	HIS_Build_button(HMI *self);

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

FUNCTION_SETTING(HMI.hitHandle, Main_HMI_hit);
FUNCTION_SETTING(HMI.build_button, HIS_Build_button);

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
//	Sheet_updown(g_p_ico_memu, h++);
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
	
}


static void HIS_btn_hdl(void *arg, uint8_t btn_id)
{
	HMI					*self	= (HMI *)arg;		
	
	switch(btn_id)
	{
			
		case ICO_ID_PGUP:
			
			break;
		case ICO_ID_PGDN:
			
			break;
	
	}

	
		
}

static void	HIS_Build_button(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	
	p->build_each_btn(0, BTN_TYPE_MENU, Main_btn_hdl, self);
	p->build_each_btn(1, BTN_TYPE_PGUP, HIS_btn_hdl, self);
	p->build_each_btn(2, BTN_TYPE_PGDN, HIS_btn_hdl, self);

}








