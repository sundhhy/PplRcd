#include "HMI_news_select.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "HMIFactory.h"
#include <string.h>

#include "HMI_striped_background.h"


//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_NewSlct_HMI;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

extern void	Hide_ico_tips(HMI *self);

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//信息选择的画面
#define NEWS_PICNUM		"7"
//static ro_char Nws_hmi_code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >26</>" };
static ro_char Nws_hmi_code_cpic[] =  {"<icon vx0=90 bx=140  xn=1 yn=3 n=0>8</>" };
//累积选择的画面
#define NEWS_ACC_PICNUM		"9"			
static ro_char Nws_hmi_code_cpic2[] =  {"<icon vx0=90 bx=140  xn=1 yn=3 n=0>10</>" };
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_Nws_HMI(HMI *self, void *arg);
static void Show_Nws_HMI(HMI *self);
static void	Nws_HMI_hide(HMI *self);
static void	Nws_initSheet(HMI *self, uint32_t att);
static void	Nws_HMI_init_focus(HMI *self);
static void	Nws_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Nws_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Nws_HMI_hitHandle( HMI *self, char kcd);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

NewSlct_HMI *Get_NewSlct_HMI(void)
{
	static NewSlct_HMI *singal_NWSHmi = NULL;
	if( singal_NWSHmi == NULL)
	{
		singal_NWSHmi = NewSlct_HMI_new();
		g_p_NewSlct_HMI = SUPER_PTR(singal_NWSHmi, HMI);

	}
	
	return singal_NWSHmi;
	
}

CTOR(NewSlct_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Nws_HMI);
FUNCTION_SETTING(HMI.initSheet, Nws_initSheet);
FUNCTION_SETTING(HMI.hide, Nws_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Nws_HMI);

FUNCTION_SETTING(HMI.build_component, Hide_ico_tips);
FUNCTION_SETTING(HMI.hitHandle, Nws_HMI_hitHandle);
FUNCTION_SETTING(HMI.init_focus, Nws_HMI_init_focus);
FUNCTION_SETTING(HMI.clear_focus, Nws_HMI_clear_focus);
FUNCTION_SETTING(HMI.show_focus, Nws_HMI_show_focus);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_Nws_HMI(HMI *self, void *arg)
{
//	NewSlct_HMI		*cthis = SUB_PTR( self, HMI, NewSlct_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
	
	self->arg[0] = 0;
	//初始化背景图片
//	p_exp = ExpCreate( "pic");
//	g_p_sht_bkpic = Sheet_alloc( p_shtctl);
//	p_exp->inptSht( p_exp, (void *)Nws_hmi_code_bkPic, g_p_sht_bkpic) ;
//	
//	g_p_cpic = Sheet_alloc( p_shtctl);
//	p_exp->inptSht( p_exp, (void *)Nws_hmi_code_cpic, g_p_cpic) ;
	
	
//	cthis->focus_row = 0;
	
	return RET_OK;
}

static void Show_Nws_HMI(HMI *self)
{
//	NewSlct_HMI		*cthis = SUB_PTR( self, HMI, NewSlct_HMI);
	g_p_sht_bkpic->p_gp->vdraw( g_p_sht_bkpic->p_gp, &g_p_sht_bkpic->cnt, &g_p_sht_bkpic->area);
	self->show_focus( self,self->p_fcuu->focus_row, 0);
}
static void	Nws_initSheet(HMI *self, uint32_t att)
{
	Expr 			*p_exp ;
	
	
	p_exp = ExpCreate( "pic");
	
	if(self->arg[0] == 0) {
		p_exp->inptSht( p_exp, (void *)Nws_hmi_code_cpic, g_p_cpic);
		g_p_sht_bkpic->cnt.data = NEWS_PICNUM;
	} else {
		
		p_exp->inptSht( p_exp, (void *)Nws_hmi_code_cpic2, g_p_cpic);
		g_p_sht_bkpic->cnt.data = NEWS_ACC_PICNUM;
	}
	
	self->init_focus(self);
}
static void	Nws_HMI_hide(HMI *self)
{
	Focus_free(self->p_fcuu);
}


static void	Nws_HMI_init_focus(HMI *self)
{
//	NewSlct_HMI		*cthis = SUB_PTR( self, HMI, NewSlct_HMI);
	int row = 0;
//	cthis->focus_row = 0;
	
	self->p_fcuu = Focus_alloc(3, 1);
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
	for(row = 0; row < 3; row ++) {
		Focus_Set_sht(self->p_fcuu, 0, row, g_p_cpic);
	}		
	
}

static void	Nws_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
//	NewSlct_HMI		*cthis = SUB_PTR( self, HMI, NewSlct_HMI);
	g_p_sht_bkpic->p_gp->vdraw( g_p_sht_bkpic->p_gp, &g_p_sht_bkpic->cnt, &g_p_sht_bkpic->area);
	Flush_LCD();
}
static void	Nws_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
//	NewSlct_HMI		*cthis = SUB_PTR( self, HMI, NewSlct_HMI);
	uint8_t			vy0[3] = {60, 100, 140};
	if(self->p_fcuu->focus_row > 2)
		return;
	g_p_cpic->area.n = self->p_fcuu->focus_row;
	
	g_p_cpic->area.y0 = vy0[self->p_fcuu->focus_row];
//	g_p_cpic->area.y1 = g_p_cpic->area.y0 + g_p_cpic->bysize;
//	g_p_cpic->area.x1 = g_p_cpic->area.x0 + g_p_cpic->bxsize;
	
	g_p_cpic->p_gp->vdraw( g_p_cpic->p_gp, &g_p_cpic->cnt, &g_p_cpic->area);
	Flush_LCD();
}


static void	Nws_HMI_hitHandle(HMI *self, char kcd)
{
	
//	NewSlct_HMI		*cthis = SUB_PTR( self, HMI, NewSlct_HMI);
//	sheet		*p_focus;
	HMI			*p_hsb;
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		chgFouse = 0;

	
	p_hsb = Create_HMI(HMI_STRIPED_BKG);
	switch(kcd)
	{

			case KEYCODE_UP:
					if(Focus_move_up(self->p_fcuu) != RET_OK)
						Focus_move_up(self->p_fcuu);
					chgFouse = 1; 
					break;
			case KEYCODE_DOWN:
				
					if(Focus_move_down(self->p_fcuu) != RET_OK)
						Focus_move_down(self->p_fcuu);
					chgFouse = 1;
					break;
			case KEYCODE_LEFT:
					 
					break;
			case KEYCODE_RIGHT:
					 
					break;

			case KEYCODE_ENTER:
					if(self->p_fcuu->focus_row >= 2)
					{
						self->switchHMI(self, g_p_HMI_menu, 0);
						return;
					}
					
					if(self->arg[0] == 0)
					{
						
						//选择信息
						p_hsb->arg[0] = HMI_SBG_SELECT_NEWS;
						p_hsb->arg[1] = self->p_fcuu->focus_row;
						self->switchHMI(self, p_hsb, 0);
						
					}
					else
					{
						//选择累积信息
						
						if(self->p_fcuu->focus_row == 0) 
						{
							
							p_hsb->arg[0] = HMI_SBG_SELECT_ACC;
							p_hsb->arg[1] = self->p_fcuu->focus_row;
							self->switchHMI(self, p_hsb, 0);
						}
						else if(self->p_fcuu->focus_row == 1) 
						{
							p_hsb->arg[0] = HMI_SBG_SELECT_ACC;
							p_hsb->arg[1] = self->p_fcuu->focus_row;
							self->switchHMI(self, p_hsb, 0);
						}
						
					}
					break;		
			case KEYCODE_ESC:
					self->switchBack(self, 0);
					break;	
			
	}
	
	
	
//	if( !strcmp( s_key, HMIKEY_UP) )
//	{
//		Focus_move_up(self->p_fcuu);
//		chgFouse = 1;
//	}
//	
//	if( !strcmp( s_key, HMIKEY_DOWN) )
//	{
//		Focus_move_down(self->p_fcuu);
//		chgFouse = 1;
//	}
//	//arg[0]用来指示当前选择的是信息画面，还是选择累积画面
//	if( !strcmp(s_key, KEYCODE_ENTER))
//	{
//		
//		if(self->p_fcuu->focus_row >= 2)
//		{
//			self->switchHMI(self, g_p_HMI_menu);
//			return;
//		}
//		
//		if(self->arg[0] == 0)
//		{
//			
//			//选择信息
//			p_hsb->arg[0] = HMI_SBG_SELECT_NEWS;
//			p_hsb->arg[1] = self->p_fcuu->focus_row;
//			self->switchHMI(self, p_hsb);
//			
//		}
//		else
//		{
//			//选择累积信息
//			
//			if(self->p_fcuu->focus_row == 0) 
//			{
//				
//				p_hsb->arg[0] = HMI_SBG_SELECT_ACC;
//				p_hsb->arg[1] = self->p_fcuu->focus_row;
//				self->switchHMI(self, p_hsb);
//			}
//			else if(self->p_fcuu->focus_row == 1) 
//			{
//				p_hsb->arg[0] = HMI_SBG_SELECT_ACC;
//				p_hsb->arg[1] = self->p_fcuu->focus_row;
//				self->switchHMI(self, p_hsb);
//			}
//			
//		}
//		
//	}

//	if( !strcmp(s_key, HMIKEY_ESC))
//	{
//		self->switchBack(self, 0);
//	}
	
	if( chgFouse)
	{	
		self->clear_focus(self, focusRow, 0);
		self->show_focus(self, 0, 0);
		
	}
	
	
	
}



