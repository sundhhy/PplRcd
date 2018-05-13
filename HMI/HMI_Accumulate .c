#include "HMI_Accumulate.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define ACCM_PICNUM		"16"
#define ACCM_DAY_TITLE		"日累积"
#define ACCM_MONTH_TITLE	"月累积"
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI 	*g_p_Accm_HMI;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
static ro_char accm_code_chntext[] =  {"<text f=16 clr=blue vx0=200 vy0=214 >通道：</>" };
static ro_char accm_code_chnboxlist[] =  {"<bu clr=blue vx0=240 vy0=212 cg=8 rg=2><text xali=m yali=m f=16 clr=blue m=0 >01</></>" };
static ro_char accm_code_info[] =  {"<text  f=16 clr=white m=0> </>" };

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_Accm_HMI(HMI *self, void *arg);
static void Show_Accm_HMI(HMI *self);
static void	Accm_HMI_hide(HMI *self);
static void	Alarm_initSheet(HMI *self, uint32_t att);
static void	Accm_HMI_init_focus(HMI *self);

static void	Accm_show_info(Accm_HMI *self, strategy_t *p);
static void	Accm_HMI_hitHandle( HMI *self, char kcd);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Accm_HMI *Get_Accm_HMI(void)
{
	static Accm_HMI *singal_Accm_HMI = NULL;
	if( singal_Accm_HMI == NULL)
	{
		singal_Accm_HMI = Accm_HMI_new();
		if(singal_Accm_HMI  == NULL) while(1);
		g_p_Accm_HMI = SUPER_PTR(singal_Accm_HMI, HMI);

	}
	
	return singal_Accm_HMI;
	
}

CTOR(Accm_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Accm_HMI);
FUNCTION_SETTING(HMI.initSheet, Alarm_initSheet);
FUNCTION_SETTING(HMI.hide, Accm_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Accm_HMI);

FUNCTION_SETTING(HMI.hitHandle, Accm_HMI_hitHandle);
FUNCTION_SETTING(HMI.init_focus, Accm_HMI_init_focus);



END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_Accm_HMI(HMI *self, void *arg)
{
	Accm_HMI		*cthis = SUB_PTR( self, HMI, Accm_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
	
	cthis->cur_chn = 0;
	
	
		
	return RET_OK;
}

static void Show_Accm_HMI(HMI *self)
{
	Accm_HMI		*cthis = SUB_PTR( self, HMI, Accm_HMI);
	Sheet_refresh(g_p_sht_bkpic);
	Accm_show_info(cthis, cthis->p_stt);
	self->show_focus( self,self->p_fcuu->focus_row, 0);
}
static void	Alarm_initSheet(HMI *self, uint32_t att)
{
	Accm_HMI		*cthis = SUB_PTR( self, HMI, Accm_HMI);
	dspContent_t	*p_cnt;
	int  			h = 0;
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	
	HMI_Ram_init();
	
	p_shtctl = GetShtctl();
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)news_cpic, g_p_cpic) ;
	
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)accm_code_chntext, g_p_text) ;
	
	p_exp = ExpCreate( "bu");
	p_exp->inptSht( p_exp, (void *)accm_code_chnboxlist, g_p_boxlist) ;
	
	cthis->p_sht_info = Sheet_alloc(p_shtctl);
	p_exp = ExpCreate( "text");
	p_exp->inptSht( p_exp, (void *)accm_code_info, cthis->p_sht_info) ;
	
	FormatSheetSub(g_p_boxlist);
	p_cnt = Button_Get_subcnt(g_p_boxlist);
	p_cnt->data = HMI_Ram_alloc(40);
	sprintf(p_cnt->data, "%02d", cthis->cur_chn);
	p_cnt->len = strlen(p_cnt->data);
	
	g_p_sht_bkpic->cnt.data = ACCM_PICNUM;
	if(self->arg[0] == 0) {
		g_p_sht_title->cnt.data = ACCM_DAY_TITLE;
//		cthis->p_stt = &g_AccDay_strategy;
	} else if(self->arg[0] == 1) {
		g_p_sht_title->cnt.data = ACCM_MONTH_TITLE;
//		cthis->p_stt = &g_AccMonth_strategy;
	}
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(g_p_text, h++);
	Sheet_updown(g_p_boxlist, h++);
//	Sheet_updown(g_p_ico_memu, h++);
	

	

	self->init_focus(self);
}
static void	Accm_HMI_hide(HMI *self)
{
	Accm_HMI		*cthis = SUB_PTR( self, HMI, Accm_HMI);

	
//	Sheet_updown(g_p_ico_memu, -1);
	Sheet_updown(g_p_boxlist, -1);
	Sheet_updown(g_p_text, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	Sheet_free(cthis->p_sht_info);
	Focus_free(self->p_fcuu);
}


static void	Accm_HMI_init_focus(HMI *self)
{
//	Accm_HMI		*cthis = SUB_PTR( self, HMI, Accm_HMI);
	self->p_fcuu = Focus_alloc(1, 2);
	
	Focus_Set_focus(self->p_fcuu, 0, 0);
	
//	Focus_Set_sht(self->p_fcuu, 0, 0, g_p_ico_memu);
	Focus_Set_sht(self->p_fcuu, 0, 1, g_p_boxlist);
	
		
}




static void	Accm_HMI_hitHandle(HMI *self, char kcd)
{
	
	Accm_HMI		*cthis = SUB_PTR( self, HMI, Accm_HMI);
	sheet		*p_focus = Focus_Get_focus(self->p_fcuu);
	dspContent_t	*p_cnt;	
	uint8_t		focusCol = self->p_fcuu->focus_col;
	uint8_t		chgFouse = 0;
	switch(kcd)
	{

			case KEYCODE_UP:
					if(p_focus == NULL) {
						goto exit;
					}
					if(p_focus->id != SHEET_BOXLIST) {
						goto exit;
					}
					if(cthis->cur_chn < phn_sys.sys_conf.num_chn) {
						cthis->cur_chn ++;
						p_cnt = Button_Get_subcnt(p_focus);
						sprintf(p_cnt->data, "%02d", cthis->cur_chn);
						p_cnt->len = strlen(p_cnt->data);
						
					}
					chgFouse = 1;			//为了刷新一下方框 
					break;
			case KEYCODE_DOWN:
					if(p_focus == NULL) {
						goto exit;
					}
					if(p_focus->id != SHEET_BOXLIST) {
						goto exit;
					}
					if(cthis->cur_chn ) {
						cthis->cur_chn --;
						p_cnt = Button_Get_subcnt(p_focus);
						sprintf(p_cnt->data, "%02d", cthis->cur_chn);
						p_cnt->len = strlen(p_cnt->data);
						
					}
					chgFouse = 1;
					break;
			case KEYCODE_LEFT:
					Focus_move_left(self->p_fcuu);
					chgFouse = 1;
					break;
			case KEYCODE_RIGHT:
					Focus_move_right(self->p_fcuu);
					chgFouse = 1; 
					break;

			case KEYCODE_ENTER:
					p_focus = Focus_Get_focus(self->p_fcuu);
					if(p_focus == NULL)
						goto exit;
					switch(p_focus->id)
					{
						case ICO_ID_MENU:
							self->switchHMI(self, g_p_HMI_menu, 0);
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
					break;		
			case KEYCODE_ESC:
					self->switchBack(self, 0);
					break;	
			
	}	
//	if( !strcmp( s_key, HMIKEY_UP) )
//	{
//		if(p_focus == NULL) {
//			goto exit;
//		}
//		if(p_focus->id != SHEET_BOXLIST) {
//			goto exit;
//		}
//		if(cthis->cur_chn < NUM_CHANNEL) {
//			cthis->cur_chn ++;
//			p_cnt = Button_Get_subcnt(p_focus);
//			sprintf(p_cnt->data, "%02d", cthis->cur_chn);
//			p_cnt->len = strlen(p_cnt->data);
//			
//		}
//		chgFouse = 1;			//为了刷新一下方框
//	}
//	
//	if( !strcmp( s_key, HMIKEY_DOWN) )
//	{
//		
//		if(p_focus == NULL) {
//			goto exit;
//		}
//		if(p_focus->id != SHEET_BOXLIST) {
//			goto exit;
//		}
//		if(cthis->cur_chn ) {
//			cthis->cur_chn --;
//			p_cnt = Button_Get_subcnt(p_focus);
//			sprintf(p_cnt->data, "%02d", cthis->cur_chn);
//			p_cnt->len = strlen(p_cnt->data);
//			
//		}
//		chgFouse = 1;
//	}

//	if( !strcmp( s_key, HMIKEY_LEFT) )
//	{
//		Focus_move_left(self->p_fcuu);
//		chgFouse = 1;
//	}
//	
//	if( !strcmp( s_key, HMIKEY_RIGHT) )
//	{
//		Focus_move_right(self->p_fcuu);
//		chgFouse = 1;
//	}
//	if( !strcmp(s_key, KEYCODE_ENTER))
//	{
//		p_focus = Focus_Get_focus(self->p_fcuu);
//		if(p_focus == NULL)
//			goto exit;
//		switch(p_focus->id)
//		{
//			case ICO_ID_MENU:
//				self->switchHMI(self, g_p_HMI_menu);
//				break;
//			case ICO_ID_PGUP:
//				break;
//			case ICO_ID_PGDN:
//				break;
//			case ICO_ID_ERASETOOL:
//				break;
//			
//			default:
//				break;
//						
//			
//		}
//		
//		
//	}
//	
//	if( !strcmp(s_key, HMIKEY_ESC))
//	{
//		self->switchBack(self, 0);
//	}
	
	if( chgFouse)
	{	
		self->clear_focus(self, 0, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
	exit:
		return;
}

static void	Accm_show_info(Accm_HMI *self, strategy_t *p)
{
	uint8_t	col = 0;
	uint8_t	len = 0;
	uint8_t	total_bxsize = 0;
	
	self->p_sht_info->area.x0 = 0;
	//显示标题
	while(1) {
		len = p->entry_txt(0, col, &self->p_sht_info->cnt.data);
		if(len == 0)
			break;
		self->p_sht_info->cnt.len = len;
		
		total_bxsize = self->p_sht_info->bxsize * len;
		self->p_sht_info->area.y0 = STRIPE_VY0 ;

		
		self->p_sht_info->p_gp->vdraw(self->p_sht_info->p_gp, &self->p_sht_info->cnt, &self->p_sht_info->area);
		self->p_sht_info->area.x0 += total_bxsize + 4 * self->p_sht_info->bxsize;

		col ++;
	}
	
}



