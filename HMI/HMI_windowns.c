#include "HMI_windows.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "focus.h"
#include "ModelTime.h"

#include "system.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_winHmi;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define	WINHMI_TITLE_CUE			"提示"
#define	WINHMI_TITLE_ALARM		"警告!"
#define	WINHMI_TITLE_ERR			"错误!"
#define	WINT_TIMER_SET				"时间设置"
#define	WINT_PSD_SET				"密码设置"
#define	WINT_PSD_INPUT				"输入密码"

static ro_char winHim_code_bkpic[] =  {"<bpic m=1 vx0=80 vy0=60 >30</>" };
static ro_char winHim_code_title[] =  {"<text m=1 f=16 clr=white vx0=88 vy0=62 > </>" };
static ro_char winHim_code_tips[] =  {"<text m=1 f=16 clr=white vx0=88 vy0=80 > </>" };
static ro_char winhmi_code_cur[] ={ "<icon vx0=96 vy0=160 xn=5 yn=1 n=0>19</>" };		//在按钮或者多选条目中的选中标识

#define CONTENT_VX0						84									//内容区
#define CONTENT_VY0						80
#define POPUP_BU_VX0				96
#define POPUP_BU_VY0				160
#define POPUP_BU_XGAP				64
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static char		*win_content;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_winHmi( HMI *self, void *arg);
static void	winHmiShow( HMI *self);
static void	WinHmi_hit( HMI *self, char kcd);
static void winHmiHide( HMI *self );
static void Win_initSheet(HMI *self, uint32_t att);


static void winHmi_InitFouse( HMI *self );


static void winHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void winHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col);
static void		WIN_Show_cmp(HMI *self);

//提示窗口还是复选窗口
static int Win_CUR_move(winHmi *cthis, int kc);

static void Popup_init(winHmi *cthis);
static void Popup_hide(winHmi *cthis);
static void PopUp_focuse(winHmi *cthis, int	f_col, int opt) ;

static void Timeset_init(winHmi *cthis);
static void Timeset_hide(winHmi *cthis);
static void Timeset_focuse(winHmi *cthis, int	f_row, int f_col, int opt) ;

static void Password_init(winHmi *cthis);
static void Password_hide(winHmi *cthis);
static void Password_focuse(winHmi *cthis, int	f_row, int f_col, int opt) ;
static void MUS_init(winHmi *cthis);
static void MUS_hide(winHmi *cthis);
static void MUS_focuse(winHmi *cthis, int	f_row, int f_col, int opt) ;
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
winHmi *Get_winHmi(void) 
{
	static winHmi *signal_winHmi;
	if( signal_winHmi == NULL)
	{
		signal_winHmi = winHmi_new();
		if(signal_winHmi  == NULL) while(1);
		g_p_winHmi = SUPER_PTR( signal_winHmi, HMI);
	}
	
	return signal_winHmi;
}

void Win_content(char *p_tips)
{
	
	win_content = p_tips;
	
}

//int WinHmi_cmd(void *p_rcv, int cmd,  void *arg)
//{
//	HMI			*self = (HMI *)p_rcv;
//	winHmi		*cthis = SUB_PTR( self, HMI, winHmi);
//	
//	switch(cmd) {
//		case sycmd_reflush:
//			self->show(self);
//			break;
//		
//		default:break;
//	}
//	
//}



CTOR(winHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_winHmi);
FUNCTION_SETTING(HMI.hide, winHmiHide);
FUNCTION_SETTING(HMI.initSheet, Win_initSheet);
FUNCTION_SETTING(HMI.show_cmp, WIN_Show_cmp);
//FUNCTION_SETTING(HMI.hmi_run, HMI_Run_none);


FUNCTION_SETTING(HMI.show, winHmiShow);
FUNCTION_SETTING(HMI.hitHandle, WinHmi_hit);

FUNCTION_SETTING(HMI.init_focus, winHmi_InitFouse);
FUNCTION_SETTING(HMI.clear_focus, winHmi_ClearFocuse);
FUNCTION_SETTING(HMI.show_focus, winHmi_ShowFocuse);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_winHmi(HMI *self, void *arg)
{
	winHmi			*cthis = SUB_PTR( self, HMI, winHmi);
	shtctl 			*p_shtctl = GetShtctl();
	
	cthis->p_sht_bkpic = Sheet_alloc(p_shtctl);
	cthis->p_sht_tips = Sheet_alloc(p_shtctl);
	cthis->p_sht_title = Sheet_alloc(p_shtctl);
	cthis->p_sht_cur = Sheet_alloc(p_shtctl);
	return RET_OK;

}




static void Win_initSheet(HMI *self, uint32_t att )
{
	winHmi			*cthis = SUB_PTR( self, HMI, winHmi);
	
	winHmiHide(self);
	
	cthis->win_type = self->arg[0];
	if(cthis->win_type == WINTYPE_TIME_SET) {
		
		Timeset_init(cthis);
	} else if(cthis->win_type == WINTYPE_PASSWORD_SET) {
		Password_init(cthis);
		
	} else if(cthis->win_type == WINTYPE_PASSWORD_INPUT) {
		Password_init(cthis);
		cthis->p_sht_title->cnt.data = WINT_PSD_INPUT;
	} else if(cthis->win_type < WINTYPE_MUS_BND) {
		Popup_init(cthis);
		
	} else {
		
		MUS_init(cthis);
	}
	
	self->init_focus(self);
	

	
	
}

static void		WIN_Show_cmp(HMI *self)
{
	
	
}

static void winHmiHide(HMI *self )
{
	winHmi			*cthis = SUB_PTR( self, HMI, winHmi);
	
	Sheet_updown(cthis->p_sht_cur, -1);
	Sheet_updown(cthis->p_sht_tips, -1);
	Sheet_updown(cthis->p_sht_title, -1);
	Sheet_updown(cthis->p_sht_bkpic, -1);
	
//	if(cthis->win_type == WINTYPE_TIME_SET) {
//		
//		Timeset_hide(cthis);
//	} else if(cthis->win_type == WINTYPE_PASSWORD_SET) {
//		Password_hide(cthis);
//		
//	}  else if(cthis->win_type == WINTYPE_PASSWORD_INPUT) {
//		Password_hide(cthis);
//		
//	} else if(cthis->win_type < WINTYPE_MUS_BND) {
//		Popup_hide(cthis);
//		
//	} else {
//		
//		MUS_hide(cthis);
//	}

	
	
	
}	
static void	winHmiShow(HMI *self )
{
	winHmi			*cthis = SUB_PTR( self, HMI, winHmi);
	short	 len;
	
	Sheet_refresh(cthis->p_sht_bkpic);
	if(self->arg[0] < WINTYPE_MUS_BND) {
		//简要提示，要考虑分行显示
		len = cthis->p_sht_tips->cnt.len;
		while(1) {
			if(cthis->p_sht_tips->cnt.len > 18)
				cthis->p_sht_tips->cnt.len = 18;
			cthis->p_sht_tips->p_gp->vdraw(cthis->p_sht_tips->p_gp, &cthis->p_sht_tips->cnt, &cthis->p_sht_tips->area);
			if(len < 17)
				break;
			cthis->p_sht_tips->area.y0 += 16;
			cthis->p_sht_tips->cnt.data += 18;
			len -= 18;
			cthis->p_sht_tips->cnt.len = len;
			
		}
		
	}
	
	self->show_focus(self, cthis->f_row, cthis->f_col);
}


//焦点操作
static void winHmi_InitFouse( HMI *self )
{
	winHmi		*cthis = SUB_PTR( self, HMI, winHmi);
	cthis->f_row = 0;
	cthis->f_col = 0;
}



static void winHmi_ClearFocuse(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	winHmi		*cthis = SUB_PTR( self, HMI, winHmi);
	if(cthis->win_type == WINTYPE_TIME_SET) {
		
		Timeset_focuse(cthis, fouse_row, fouse_col, 0);
	} else if(cthis->win_type == WINTYPE_PASSWORD_SET) {
		Password_focuse(cthis, fouse_row, fouse_col, 0);
		
	} else if(cthis->win_type == WINTYPE_PASSWORD_INPUT) {
		Password_focuse(cthis, fouse_row, fouse_col, 0);
		
	} else if(self->arg[0] < WINTYPE_MUS_BND) {
		
		PopUp_focuse(cthis, fouse_col, 0);
	} else {
		
		MUS_focuse(cthis, fouse_row, fouse_col, 0);
	}

}
static void winHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	winHmi		*cthis = SUB_PTR( self, HMI, winHmi);
	if(cthis->win_type == WINTYPE_TIME_SET) {
		
		Timeset_focuse(cthis, fouse_row, fouse_col, 1);
	} else if(cthis->win_type == WINTYPE_PASSWORD_SET) {
		Password_focuse(cthis, fouse_row, fouse_col, 1);
		
	} else if(cthis->win_type == WINTYPE_PASSWORD_INPUT) {
		Password_focuse(cthis, fouse_row, fouse_col, 1);
		
	}  else if(self->arg[0] < WINTYPE_MUS_BND) {
		
		PopUp_focuse(cthis, fouse_row, 1);
	} else {
		
		MUS_focuse(cthis, cthis->f_row, cthis->f_col, 1);
	}
}

static void	WinHmi_hit(HMI *self, char kcd)
{
	winHmi		*cthis = SUB_PTR( self, HMI, winHmi);
//	HMI			*p_src_hmi;

	uint8_t		focusRow = cthis->f_row;
	uint8_t		focusCol = cthis->f_col;
	char			chgFouse = 0;
	

	
	switch(kcd)
	{
		
		case KEYCODE_UP:
			if(Win_CUR_move(cthis, KEYCODE_UP) == RET_OK) {
				chgFouse = 1;
			}
			break;
		case KEYCODE_DOWN:
			if(Win_CUR_move(cthis, KEYCODE_DOWN) == RET_OK) {
				chgFouse = 1;
			}
			break;
		
		case KEYCODE_LEFT:
			if(Win_CUR_move(cthis, KEYCODE_LEFT) == RET_OK) {
				chgFouse = 1;
			}
			break;
		case KEYCODE_RIGHT:
			if(Win_CUR_move(cthis, KEYCODE_RIGHT) == RET_OK) {
				chgFouse = 1;
			}
			break;
		
		
		case KEYCODE_ENTER:
			if((cthis->win_type & WINTYPE_SETTING) && cthis->f_row == 0) {
				//时间设置窗口有2行
				//确认时，切换到第二行
				cthis->f_row = 1;
				cthis->f_col = 0;
				chgFouse = 1;
			} else if((self->arg[1] &WINFLAG_RETURN) ){
				
//				p_src_hmi = g_p_win_last;
//				p_src_hmi->flag |= HMIFLAG_WIN;
				if((self->arg[1] &WINFLAG_COMMIT) && (cthis->f_col == 0))
				{
					
					cthis->cmd_hdl(cthis->p_cmd_rcv, wincmd_commit, NULL);
				}
				self->arg[1] = 0;
//				p_src_hmi->flag |= HMI_FLAG_KEEP;
//				self->switchHMI(self, p_src_hmi, HMI_ATT_KEEP);
//				p_src_hmi->flag &= ~HMIFLAG_WIN;
				
				self->switchBack(self, HMI_ATT_KEEP);
				
			} else {

				if(cthis->f_col == 0)		//确认按键
					cthis->cmd_hdl(cthis->p_cmd_rcv, wincmd_commit, NULL);
				else {
					//取消则直接返回
//					p_src_hmi = g_p_win_last;
//					p_src_hmi->flag |= HMIFLAG_WIN;
//					p_src_hmi->flag |= HMI_FLAG_KEEP;
//					self->switchHMI(self, p_src_hmi,  HMI_ATT_KEEP);
//					p_src_hmi->flag &= ~HMIFLAG_WIN;
					
					self->switchBack(self, HMI_ATT_KEEP);
				}
					
			}
			break;		
		case KEYCODE_ESC:
			
			
//			p_src_hmi = g_p_win_last;
//			p_src_hmi->flag |= HMIFLAG_WIN;
//			p_src_hmi->flag |= HMI_FLAG_KEEP;
//			self->switchHMI(self, p_src_hmi);
//			p_src_hmi->flag &= ~HMIFLAG_WIN;
		
		
			self->switchBack(self, HMI_ATT_KEEP);
			break;	
		
	}
	
	
	if( chgFouse)
	{
			
		self->clear_focus(self, focusRow, focusCol);
		self->show_focus(self, cthis->f_row, cthis->f_col);
		
	}
	
}


//todo:这这里的if语句替换掉

static int Win_CUR_move(winHmi *cthis, int kc)
{
	int ret = RET_OK;
	if(cthis->win_type < WINTYPE_MUS_BND) {
		if(cthis->f_col == 0)
			cthis->f_col = 1;
		else 
			cthis->f_col = 0;
		
	} else if(cthis->win_type == WINTYPE_TIME_SET){
		switch(kc) {
			case KEYCODE_UP:
				if(cthis->f_row == 0) {
					MdlTime_text_modify(win_content, cthis->f_col, OP_ADD);
					
				}
				else 
					cthis->f_row = 0;
				break;
			case KEYCODE_DOWN:
				if(cthis->f_row == 0) {
					MdlTime_text_modify(win_content, cthis->f_col, OP_SUB);
				}
				else 
					cthis->f_row = 0;
				break;
			case KEYCODE_LEFT:
				if(cthis->f_row == 0) {
					cthis->f_col = MdlTime_text_iteartor(win_content, cthis->f_col, 0);
				} else {
					if(cthis->f_col == 0)
						cthis->f_col = 1;
					else 
						cthis->f_col = 0;
				}
				break;
			case KEYCODE_RIGHT:
				if(cthis->f_row == 0) {
					cthis->f_col = MdlTime_text_iteartor(win_content, cthis->f_col, 1);
				} else {
					if(cthis->f_col == 0)
						cthis->f_col = 1;
					else 
						cthis->f_col = 0;
					
				}
				break;
			
			
		}
		
	} else if((cthis->win_type == WINTYPE_PASSWORD_SET) || (cthis->win_type == WINTYPE_PASSWORD_INPUT)){
		switch(kc) {
			case KEYCODE_UP:
				if(cthis->f_row == 0) {
					Password_modify(win_content, cthis->f_col, OP_ADD);
					
				}
				else 
					cthis->f_row = 0;
				break;
			case KEYCODE_DOWN:
				if(cthis->f_row == 0) {
					Password_modify(win_content, cthis->f_col, OP_SUB);
				}
				else 
					cthis->f_row = 0;
				break;
			case KEYCODE_LEFT:
				if(cthis->f_row == 0) {
					cthis->f_col = Password_iteartor(win_content, cthis->f_col, 0);
				} else {
					if(cthis->f_col == 0)
						cthis->f_col = 1;
					else 
						cthis->f_col = 0;
				}
				break;
			case KEYCODE_RIGHT:
				if(cthis->f_row == 0) {
					cthis->f_col = Password_iteartor(win_content, cthis->f_col, 1);
				} else {
					if(cthis->f_col == 0)
						cthis->f_col = 1;
					else 
						cthis->f_col = 0;
					
				}
				break;
			
			
		}
		
	} else {
		//复选框的光标移动
		
	}
	
		
		
	
	
	return ret;
	
}

static void Popup_init(winHmi *cthis)
{
	Expr 				*p_exp ;
	shtctl 			*p_shtctl = GetShtctl();
	int 				h = 0;
	p_exp = ExpCreate( "pic");
//	cthis->p_sht_bkpic = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winHim_code_bkpic, cthis->p_sht_bkpic) ;
	
	
	p_exp = ExpCreate( "text");
//	cthis->p_sht_tips = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winHim_code_tips, cthis->p_sht_tips);
	cthis->p_sht_tips->cnt.data = win_content;
	cthis->p_sht_tips->cnt.len = strlen(cthis->p_sht_tips->cnt.data);
	
	
	p_exp = ExpCreate( "text");
//	cthis->p_sht_title = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winHim_code_title, cthis->p_sht_title) ;
	switch(cthis->win_type)
	{
		case WINTYPE_TIPS:
			cthis->p_sht_title->cnt.data = WINHMI_TITLE_CUE;
			break;
		case WINTYPE_ALARM:
			cthis->p_sht_title->cnt.data = WINHMI_TITLE_ALARM;
			break;
		case WINTYPE_ERROR:
			cthis->p_sht_title->cnt.data = WINHMI_TITLE_ERR;
			cthis->p_sht_title->cnt.colour = COLOUR_RED;
			cthis->p_sht_tips->cnt.colour = COLOUR_RED;
			break;
		default:
			cthis->p_sht_title->cnt.data = WINHMI_TITLE_CUE;
			break;
	}
	cthis->p_sht_title->cnt.len = strlen(cthis->p_sht_title->cnt.data);
	
	
	p_exp = ExpCreate( "pic");
//	cthis->p_sht_cur = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winhmi_code_cur, cthis->p_sht_cur);

	Sheet_updown(cthis->p_sht_bkpic, h++);
	Sheet_updown(cthis->p_sht_title, h++);
//	Sheet_updown(cthis->p_sht_tips, h++);
//	Sheet_updown(cthis->p_sht_cur, h++);
	
}

static void Popup_hide(winHmi *cthis)
{
//	Sheet_updown(cthis->p_sht_cur, -1);
//	Sheet_updown(cthis->p_sht_tips, -1);
	Sheet_updown(cthis->p_sht_title, -1);
	Sheet_updown(cthis->p_sht_bkpic, -1);
//	
//	Sheet_free(cthis->p_sht_cur);
//	Sheet_free(cthis->p_sht_tips);
//	Sheet_free(cthis->p_sht_title);
//	Sheet_free(cthis->p_sht_bkpic);
	
}



//提示窗口:只有确定与取消两个按键
static void PopUp_focuse(winHmi *cthis, int	f_col, int opt) 
{
	if(opt == 0) {
		cthis->p_sht_cur->area.n = 4;
		
	} else {
		
		cthis->p_sht_cur->area.n = 0;
		f_col = cthis->f_col;
	}
	cthis->p_sht_cur->area.x0 = POPUP_BU_VX0 + f_col * POPUP_BU_XGAP;
	cthis->p_sht_cur->area.y0 = POPUP_BU_VY0;
	
	cthis->p_sht_cur->p_gp->vdraw(cthis->p_sht_cur->p_gp, &cthis->p_sht_cur->cnt, &cthis->p_sht_cur->area);
	Flush_LCD();
}



static void Timeset_init(winHmi *cthis)
{
	Expr 				*p_exp ;
	shtctl 			*p_shtctl = GetShtctl();
	int 				h = 0;
	p_exp = ExpCreate("pic");
//	cthis->p_sht_bkpic = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winHim_code_bkpic, cthis->p_sht_bkpic) ;
	
	
	p_exp = ExpCreate("text");
//	cthis->p_sht_tips = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winHim_code_tips, cthis->p_sht_tips);
	cthis->p_sht_tips->area.x0 = CONTENT_VX0;
	cthis->p_sht_tips->area.y0 = CONTENT_VY0 + 16;
	cthis->p_sht_tips->cnt.data = win_content;
	cthis->p_sht_tips->cnt.len = strlen(cthis->p_sht_tips->cnt.data);
	
	
	p_exp = ExpCreate( "text");
//	cthis->p_sht_title = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)winHim_code_title, cthis->p_sht_title);

	cthis->p_sht_title->cnt.data = WINT_TIMER_SET;
	cthis->p_sht_title->cnt.len = strlen(cthis->p_sht_title->cnt.data);
	
	
	p_exp = ExpCreate( "pic");
//	cthis->p_sht_cur = Sheet_alloc(p_shtctl);
	p_exp->inptSht(p_exp, (void *)winhmi_code_cur, cthis->p_sht_cur);

	Sheet_updown(cthis->p_sht_bkpic, h++);
	Sheet_updown(cthis->p_sht_title, h++);
	Sheet_updown(cthis->p_sht_tips, h++);
	
}
static void Timeset_hide(winHmi *cthis)
{
	Sheet_updown(cthis->p_sht_tips, -1);
	Sheet_updown(cthis->p_sht_title, -1);
	Sheet_updown(cthis->p_sht_bkpic, -1);
	
//	Sheet_free(cthis->p_sht_cur);
//	Sheet_free(cthis->p_sht_tips);
//	Sheet_free(cthis->p_sht_title);
//	Sheet_free(cthis->p_sht_bkpic);
	
}




static void Timeset_focuse(winHmi *cthis, int	f_row, int f_col, int opt) 
{
	uint16_t	txt_xsize, txt_ysize;
	cthis->p_sht_tips->p_gp->getSize(cthis->p_sht_tips->p_gp, cthis->p_sht_tips->cnt.font, &txt_xsize, &txt_ysize);
	
	
	if(f_row == 0) {
		//在时间设置行
		//对选中的字符进行高亮
		cthis->p_sht_tips->p_gp->getSize(cthis->p_sht_tips->p_gp, cthis->p_sht_tips->cnt.font, &txt_xsize, &txt_ysize);
		if(opt)
			cthis->p_sht_tips->cnt.colour = COLOUR_BLUE;
		else
			cthis->p_sht_tips->cnt.colour = COLOUR_WHITE;
		
		cthis->p_sht_tips->cnt.data = win_content + f_col;
		cthis->p_sht_tips->cnt.len = 1;
		cthis->p_sht_tips->area.x0 = CONTENT_VX0 + txt_xsize * f_col;
		cthis->p_sht_tips->p_gp->vdraw(cthis->p_sht_tips->p_gp, &cthis->p_sht_tips->cnt, &cthis->p_sht_tips->area);
		
		
	} else {
		//在选择框
		if(opt == 0) {
		cthis->p_sht_cur->area.n = 4;
			
		} else {
			
			cthis->p_sht_cur->area.n = 0;
			f_col = cthis->f_col;
		}
		cthis->p_sht_cur->area.x0 = POPUP_BU_VX0 + f_col * POPUP_BU_XGAP;
		cthis->p_sht_cur->area.y0 = POPUP_BU_VY0;
		cthis->p_sht_cur->p_gp->vdraw(cthis->p_sht_cur->p_gp, &cthis->p_sht_cur->cnt, &cthis->p_sht_cur->area);
		
	}
	
	Flush_LCD();
	
}

//密码设置和时间设置在显示上是一样的处理,即显示win_content
//只有对具体的数值的设置，才有所不同
static void Password_init(winHmi *cthis)
{
	Timeset_init(cthis);
	cthis->p_sht_title->cnt.data = WINT_PSD_SET;
	cthis->p_sht_title->cnt.len = strlen(cthis->p_sht_title->cnt.data);
}
static void Password_hide(winHmi *cthis)
{
	Timeset_hide(cthis);
	
}
static void Password_focuse(winHmi *self, int	f_row, int f_col, int opt) 
{
	Timeset_focuse(self, f_row, f_col, opt);
	
}



//多选窗口

static void MUS_init(winHmi *cthis)
{
	
	
}
static void MUS_hide(winHmi *cthis)
{
	
	
}
static void MUS_focuse(winHmi *self, int	f_row, int f_col, int opt) 
{
	
	
}











