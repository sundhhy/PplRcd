#include "HMI_setup.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "HMIFactory.h"
#include "HMI_windows.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define SETUP_PICNUM		"17"
#define SETUP_TITLE		"设置"

#define PSD_LOCKED		"未解锁"
#define PSD_UNLOCK		"解锁"

static const char setup_hmi_code_clean[] =  {"<cpic  bx=160 by=40>17</>" };

static const char setup_hmi_code_cpic[] =  {"<icon bx=160 by=40 xn=2 yn=4 n=0>18</>" };
static const char setup_hmi_code_passwd[] =  {"<text vx0=100 vy0=44 f=24 clr=blue m=0> </>" };
static const char setup_hmi_code_lock[] =  {"<text vx0=230 vy0=50 f=16 m=0> </>" };

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//HMI 	*g_p_Setup_HMI;
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
static void STP_Run(HMI *self);

static void	Setup_initSheet(HMI *self, uint32_t att);
static void	Setup_HMI_init_focus(HMI *self);
static void	Setup_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Setup_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void	Setup_HMI_hitHandle( HMI *self, char kcd);
static void	STP_Compose_hit( HMI *self, char kcd_1, char kcd_2);



static void Input_Password(HMI *self);
static void Setup_HMI_lock(Setup_HMI		*cthis);
static void Setup_HMI_unlock(Setup_HMI		*cthis);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

Setup_HMI *Get_Setup_HMI(void)
{
	static Setup_HMI *singal_Setup_HMI = NULL;
	if( singal_Setup_HMI == NULL)
	{
		singal_Setup_HMI = Setup_HMI_new();
		if(singal_Setup_HMI  == NULL) while(1);
//		g_p_Setup_HMI = SUPER_PTR(singal_Setup_HMI, HMI);

	}
	
	return singal_Setup_HMI;
	
}

CTOR(Setup_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Setup_HMI);
FUNCTION_SETTING(HMI.initSheet, Setup_initSheet);
FUNCTION_SETTING(HMI.hide, Setup_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Setup_HMI);
FUNCTION_SETTING(HMI.hmi_run, STP_Run);


FUNCTION_SETTING(HMI.hitHandle, Setup_HMI_hitHandle);
FUNCTION_SETTING(HMI.conposeKeyHandle, STP_Compose_hit);


FUNCTION_SETTING(HMI.init_focus, Setup_HMI_init_focus);
FUNCTION_SETTING(HMI.clear_focus, Setup_HMI_clear_focus);
FUNCTION_SETTING(HMI.show_focus, Setup_HMI_show_focus);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
static void STP_Run(HMI *self)
{
	
	HMI_Updata_tip_ico();
}
static int	Init_Setup_HMI(HMI *self, void *arg)
{
//	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
//	

//	p_exp = ExpCreate( "pic");
//	
		
	return RET_OK;
}

static void Show_Setup_HMI(HMI *self)
{
//	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	Sheet_refresh(g_p_sht_bkpic);
}
static void	Setup_initSheet(HMI *self, uint32_t att)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	int  				h = 0;
	Expr 				*p_exp ;
	shtctl 			*p_shtctl = NULL;
	p_shtctl = GetShtctl();
	
	
	
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_cpic, g_p_cpic) ;
	
	cthis->p_clean_focus = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_clean, cthis->p_clean_focus) ;
	
	//密码
	p_exp = ExpCreate( "text");
	cthis->p_password = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_passwd, cthis->p_password) ;
	cthis->p_password->id = SHEET_PSD_TEXT;
	p_exp = ExpCreate("text");
	cthis->p_lock = Sheet_alloc( p_shtctl);
	p_exp->inptSht( p_exp, (void *)setup_hmi_code_lock, cthis->p_lock) ;

	
		
	HMI_Ram_init();
	arr_p_vram[0] = HMI_Ram_alloc(4);				//用来放数值的密码
	arr_p_vram[1] = HMI_Ram_alloc(16);				//用来放密码对应的字符串
	memset(arr_p_vram[0], 0, 4);
	Print_sys_param(arr_p_vram[0], arr_p_vram[1] , 16, es_psd); 
		
		

	if((att & HMI_ATT_KEEP) == 0)
	{		
		//从其他画面切换进入设置选择画面的时候，需要重新输入密码
		Setup_HMI_lock(cthis);
		
	} else {
		
			//密码输入窗口的结果来设置界面
			//设置画面返回时就不必重新输入密码了
		
		if(cthis->unlock) {
			Setup_HMI_lock(cthis);
		} else {
			
			Setup_HMI_unlock(cthis);
		}
		
		
	}
	cthis->p_password->cnt.data = arr_p_vram[1];
	cthis->p_password->cnt.len = strlen(cthis->p_password->cnt.data);

	g_p_sht_bkpic->cnt.data = SETUP_PICNUM;

	
	g_p_sht_title->cnt.data = SETUP_TITLE;
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(cthis->p_password, h++);
	Sheet_updown(cthis->p_lock, h++);

	self->init_focus(self);
}
static void	Setup_HMI_hide(HMI *self)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	
	
	
	Sheet_updown(cthis->p_lock, -1);
	Sheet_updown(cthis->p_password, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	Sheet_free(cthis->p_clean_focus);
	Sheet_free(cthis->p_password);
	Sheet_free(cthis->p_lock);
	
	Focus_free(self->p_fcuu);
}


static void	Setup_HMI_init_focus(HMI *self)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	short i;
	
	if(cthis->unlock) {
		self->p_fcuu = Focus_alloc(1, 1);
		Focus_Set_sht(self->p_fcuu, 0, 0, cthis->p_password);
		Focus_Set_focus(self->p_fcuu, 1, 0);
	} else {
		self->p_fcuu = Focus_alloc(4, 2);
		for(i = 0; i < 4; i++) {
		
			Focus_Set_sht(self->p_fcuu, i, 0, g_p_sht_bkpic);
			Focus_Set_sht(self->p_fcuu, i, 1, g_p_sht_bkpic);
		}
		
		Focus_Set_focus(self->p_fcuu, 4, 0);
	}

//	Focus_Set_focus(self->p_fcuu, 5, 0);
	
//	self->p_fcuu = Focus_alloc(5, 2);
//	
//	Focus_Set_sht(self->p_fcuu, 0, 0, cthis->p_password);
//	Focus_Set_sht(self->p_fcuu, 0, 1, cthis->p_password);
//	
//	for(i = 1; i < 5; i++) {
//		
//		Focus_Set_sht(self->p_fcuu, i, 0, g_p_sht_bkpic);
//		Focus_Set_sht(self->p_fcuu, i, 1, g_p_sht_bkpic);
//	}
//	
//	Focus_Set_focus(self->p_fcuu, 5, 0);
}

static void	Setup_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	uint8_t			vx0 = 0;
	uint8_t			vy0 = 80;
	
	if(cthis->unlock ) {
		if(fouse_row || fouse_col) 
			return;
		cthis->p_password->cnt.effects = GP_CLR_EFF(cthis->p_password->cnt.effects, EFF_FOCUS);
		Sheet_slide(cthis->p_password);
	} else if(fouse_row > 3 || fouse_col >1) {
		return;
	} else {
		cthis->p_clean_focus->area.x0 = vx0 + fouse_col * cthis->p_clean_focus->bxsize;
		cthis->p_clean_focus->area.y0 = vy0 + (fouse_row - 0) *  cthis->p_clean_focus->bysize;
		
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
	if(cthis->unlock ) {
		if(row || col) 
			return;
		cthis->p_password->cnt.effects = GP_SET_EFF(cthis->p_password->cnt.effects, EFF_FOCUS);
		Sheet_slide(cthis->p_password);
	} else if(row > 3 || col >1) {
		return;
	} else {
		g_p_cpic->area.n = (row - 0) * 2 + col;
		g_p_cpic->area.x0 = vx0 + col * g_p_cpic->bxsize;
		g_p_cpic->area.y0 = vy0 + (row - 0) * g_p_cpic->bysize;
		g_p_cpic->p_gp->vdraw( g_p_cpic->p_gp, &g_p_cpic->cnt, &g_p_cpic->area);
		Flush_LCD();
	}
	

}


static void	Setup_HMI_hitHandle(HMI *self, char kcd)
{
	
	Setup_HMI	*cthis = SUB_PTR( self, HMI, Setup_HMI);
	sheet		*p_focus;
	HMI			*p_hsb;
	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = self->p_fcuu->focus_col;
	uint8_t		chgFouse = 0;
	
	
	p_hsb = Create_HMI(HMI_STRIPED_BKG);


	switch(kcd)
	{

			case KEYCODE_UP:
					if(Focus_move_up(self->p_fcuu) != RET_OK)
						Focus_move_up(self->p_fcuu);		//焦点不允许出现在外部
					chgFouse = 1;
					break;
			case KEYCODE_DOWN:
					if(Focus_move_down(self->p_fcuu) != RET_OK)
						Focus_move_down(self->p_fcuu);
					chgFouse = 1;					
					break;
			case KEYCODE_LEFT:
					if(cthis->unlock)
					{
						self->switchBack(self, 0);
						goto exit;
						
					}
					if(Focus_move_left(self->p_fcuu) != RET_OK)
						Focus_move_left(self->p_fcuu);
					chgFouse = 1;					 
					break;
			case KEYCODE_RIGHT:
					if(cthis->unlock)
					{
						self->switchBack(self, 0);
						goto exit;
						
					}
			
			
					if(Focus_move_right(self->p_fcuu) != RET_OK)
						Focus_move_right(self->p_fcuu);
					chgFouse = 1;					 
					break;

			case KEYCODE_ENTER:
					p_focus = Focus_Get_focus(self->p_fcuu);
					if(p_focus == NULL) 
							goto exit;
					
					if(p_focus->id == SHEET_PSD_TEXT) {
						Input_Password(self);
						goto exit;
					} else if(cthis->unlock == 0){
						
						if(self->p_fcuu->focus_row == 3 && self->p_fcuu->focus_col) {
							self->switchHMI(self, g_p_HMI_menu, HMI_ATT_NOT_RECORD);
						} 
						else 
						{
							p_hsb->arg[0] = self->p_fcuu->focus_row - 0;
							p_hsb->arg[1] = self->p_fcuu->focus_col;
							
							self->switchHMI(self, p_hsb, 0);
						}
					}					
					break;		
			case KEYCODE_ESC:
					self->switchHMI(self, Create_HMI(HMI_STRIPED_BKG), 0);
					break;	
			
	}


	
	if( chgFouse)
	{	
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
	exit:
		return;
}


static void	STP_Compose_hit( HMI *self, char kcd_1, char kcd_2)
{
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	
	if(cthis->unlock == 0)
		return;		//已经解锁了，就只能通过退出按钮退出
	
	if(kcd_2 == KEYCODE_UP && kcd_1 == KEYCODE_DOWN) {
//		self->switchHMI(self, Create_HMI(HMI_STRIPED_BKG), HMI_ATT_NOT_RECORD);
		self->switchBack(self, 0);
	}
	else if(kcd_2 == KEYCODE_DOWN && kcd_1 == KEYCODE_UP) {
//		self->switchHMI(self, Create_HMI(HMI_STRIPED_BKG), HMI_ATT_NOT_RECORD);
		self->switchBack(self, 0);
	}
	
}


static void Setup_HMI_unlock(Setup_HMI		*cthis)
{
	cthis->p_lock->cnt.data = PSD_UNLOCK;
	cthis->p_lock->cnt.len = strlen(cthis->p_lock->cnt.data);
	cthis->p_lock->cnt.colour = COLOUR_GREN;
	cthis->unlock = 0;
	
}
static void Setup_HMI_lock(Setup_HMI		*cthis)
{
	cthis->p_lock->cnt.data = PSD_LOCKED;
	cthis->p_lock->cnt.len = strlen(cthis->p_lock->cnt.data);
	cthis->p_lock->cnt.colour = COLOUR_YELLOW;
	cthis->unlock = 1;
	
}


static int Setup_HMI_cmd(void *p_rcv, int cmd,  void *arg)
{
	HMI					*self = (HMI *)p_rcv;
	HMI					*p_hsb = Create_HMI(HMI_STRIPED_BKG);
	Setup_HMI		*cthis = SUB_PTR( self, HMI, Setup_HMI);
	int 				ret = RET_OK;
	char				win_tips[32];
	char				super_unlock = 0;
	
	switch(cmd) {
		
		case wincmd_commit:
			//先匹配超级密码，匹配成功就解锁，并且进入超级设置界面
			ret = Str_Password_match(arr_p_vram[1], phn_sys.sys_conf.super_psd) ;
			if(ret != RET_OK)
				ret = Str_Password_match(arr_p_vram[1], phn_sys.sys_conf.password) ;
			else
				super_unlock = 1;
			
			if(ret == 0) {
				g_p_winHmi->arg[0] = WINTYPE_TIPS;
				g_p_winHmi->arg[1] = WINFLAG_RETURN;
				Win_content("密码输入成功");
				Setup_HMI_unlock(cthis);
				self->switchHMI(self, g_p_winHmi, HMI_ATT_NOT_RECORD);
				if(super_unlock)
				{
					
					p_hsb->arg[0] = HMI_SBG_SUPER_SET_ROW;
					p_hsb->arg[1] = HMI_SBG_SUPER_SET_COL;
					self->switchHMI(self, p_hsb, 0);
				}
//				g_p_winHmi->switchHMI(g_p_winHmi, g_p_winHmi);
			} else {
				g_p_winHmi->arg[0] = WINTYPE_ERROR;
				g_p_winHmi->arg[1] = WINFLAG_RETURN;
				sprintf(win_tips,"密码错误");
				Win_content(win_tips);
				Setup_HMI_lock(cthis);
//				g_p_winHmi->switchHMI(g_p_winHmi, g_p_winHmi);
				self->switchHMI(self, g_p_winHmi, HMI_ATT_NOT_RECORD);

			}
		
			break;
		
	}
	
	return ret;
	
	
}


static void Input_Password(HMI *self)
{
	winHmi			*p_win;
	Win_content(arr_p_vram[1]);
	g_p_winHmi->arg[0] = WINTYPE_PASSWORD_INPUT;
	g_p_winHmi->arg[1] = 0;
	p_win = Get_winHmi();
	p_win->p_cmd_rcv = self;
	p_win->cmd_hdl = Setup_HMI_cmd;
	self->switchHMI(self, g_p_winHmi, 0);
	
	
}



