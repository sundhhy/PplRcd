#include "Setting_HMI.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "windowsHmi.h"

#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define SETTING_PICNUM		"16"
#define SETTING_TITLE		"设置"

static const char setting_hmi_code_text[] =  {"<text  f=16 clr=white m=0> </>" };
static const char setting_hmi_code_CUR[] =  {"<box clr=gren> </>" };
static const char setting_hmi_code_clean[] =  {"<cpic>16</>" };


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
 {&g_view_strategy, &g_DBP_strategy},{&g_dataPrint_strategy, NULL}};
 
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
static void	Setting_HMI_dhit( HMI *self, char *s_key);
static void	Setting_HMI_long_hit( HMI *self, char *s_key);
static void	Setting_HMI_build_component(HMI *self);

static void	Show_entry(HMI *self, strategy_t *p_st);
static int STING_Show_Button(HMI *self, int up_or_dn);
 
 
static sheet* Setting_HMI_get_focus(Setting_HMI *self, int arg);
static void Strategy_focus(Setting_HMI *self, strategy_focus_t *p_syf, int opt);
static int Setting_Sy_cmd(void *p_rcv, int cmd,  void *arg);
 


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

//设置策略的默认函数
void STY_Duild_button(void *arg)
{
	Button	*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	
}
void Setting_btn_hdl(void *arg, uint8_t btn_id)
{
	HMI					*self	= (HMI *)arg;		
	Setting_HMI			*cthis = SUB_PTR(self, HMI, Setting_HMI);
	
	if(btn_id == ICO_ID_MENU)
	{
		
		cthis->entry_start_row = 0;
		self->switchHMI(self, g_p_HMI_menu);
	}
	else if((btn_id == ICO_ID_PGUP) || (btn_id == ICO_ID_PGDN))
	{
		STING_Show_Button(self, btn_id);
	}
}

CTOR(Setting_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Setting_HMI);
FUNCTION_SETTING(HMI.initSheet, Setting_initSheet);
FUNCTION_SETTING(HMI.hide, Setting_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Setting_HMI);

FUNCTION_SETTING(HMI.hitHandle, Setting_HMI_hitHandle);
FUNCTION_SETTING(HMI.dhitHandle, Setting_HMI_dhit);
FUNCTION_SETTING(HMI.longpushHandle, Setting_HMI_long_hit);


FUNCTION_SETTING(HMI.init_focus, Setting_HMI_init_focus);
FUNCTION_SETTING(HMI.clear_focus, Setting_HMI_clear_focus);
FUNCTION_SETTING(HMI.show_focus, Setting_HMI_show_focus);

FUNCTION_SETTING(HMI.build_component, Setting_HMI_build_component);
END_CTOR



//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_Setting_HMI(HMI *self, void *arg)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
//	

//	p_exp = ExpCreate( "pic");
	
	cthis->entry_start_row = 0;	
	phn_sys.hmi_mgr.set_strategy = 0xff;
	return RET_OK;
}

static void Show_Setting_HMI(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);

	Stop_flush_LCD();
	Sheet_refresh(g_p_sht_bkpic);
	Show_entry(self, cthis->p_sy);
	Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	Flush_LCD();
	
}
static void	Setting_initSheet(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	int  		 		h = 0;
	Expr 				*p_exp ;
	shtctl 				*p_shtctl = NULL;
	strategy_t			*old_sty;
	p_shtctl = GetShtctl();
	
	if(((self->flag & HMIFLAG_WIN) == 0) && ((self->flag & HMIFLAG_KEYBOARD) == 0)) {
		old_sty = cthis->p_sy;
		cthis->p_sy = arr_p_setting_strategy[self->arg[0]][self->arg[1]];
		
		
		if(old_sty && old_sty != cthis->p_sy)
			old_sty->exit();
		
		cthis->p_sy->p_cmd_rcv = self;
		cthis->p_sy->cmd_hdl = Setting_Sy_cmd;
		
	}
	

	
	p_exp = ExpCreate( "text");
	cthis->p_sht_text = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_text, cthis->p_sht_text) ;
	cthis->p_sht_text->input = NULL;
	
	p_exp = ExpCreate( "box");
	cthis->p_sht_CUR = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_CUR, cthis->p_sht_CUR) ;
	
	p_exp = ExpCreate( "pic");
	cthis->p_sht_clean = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_clean, cthis->p_sht_clean) ;


	g_p_sht_bkpic->cnt.data = SETTING_PICNUM;

	
	g_p_sht_title->cnt.data = setting_titles[self->arg[0]][self->arg[1]];
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
//	Sheet_updown(g_p_ico_memu, h++);
	

	self->init_focus(self);
}
static void	Setting_HMI_hide(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
//	Sheet_updown(g_p_ico_memu, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	Sheet_free(cthis->p_sht_text);
	Sheet_free(cthis->p_sht_CUR);
	Sheet_free(cthis->p_sht_clean);
	
	phn_sys.hmi_mgr.set_strategy = 0xff;
}


static void	Setting_HMI_init_focus(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	cthis->f_col = 0;
	cthis->sub_flag &= 0xf0;
	cthis->col_max = 1;
	if(((self->flag & HMIFLAG_WIN) == 0) && ((self->flag & HMIFLAG_KEYBOARD) == 0))
		cthis->p_sy->init(NULL);
	phn_sys.hmi_mgr.set_strategy = cthis->p_sy->sty_id;
	SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
	
	
//	g_p_ico_pgup->cnt.effects = GP_CLR_EFF(g_p_ico_pgup->cnt.effects, EFF_FOCUS);
//	g_p_ico_pgdn->cnt.effects = GP_CLR_EFF(g_p_ico_pgdn->cnt.effects, EFF_FOCUS);
//	g_p_ico_memu->cnt.effects = GP_CLR_EFF(g_p_ico_memu->cnt.effects, EFF_FOCUS);
	

}

static void	Setting_HMI_build_component(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	
	cthis->p_sy->build_component(self);
}

static void	Setting_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
//	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
//	sheet *p_fouse = Setting_HMI_get_focus(cthis, fouse_col);
//	
//	if(p_fouse == NULL)
//		return;
//	p_fouse->cnt.effects = GP_CLR_EFF(p_fouse->cnt.effects, EFF_FOCUS);
//	
//	p_fouse->e_heifht = 1;
//	Sheet_slide(p_fouse);
//	p_fouse->e_heifht = 0;
	
	self->btn_jumpout(self);

}
static void	Setting_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	
	self->btn_forward(self);
//	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
//	sheet *p_fouse = Setting_HMI_get_focus(cthis, -1);
//	
//	if(p_fouse == NULL)
//		return;
//	p_fouse->cnt.effects = GP_SET_EFF(p_fouse->cnt.effects, EFF_FOCUS);
//	p_fouse->e_heifht = 1;
//	Sheet_slide(p_fouse);
//	p_fouse->e_heifht = 0;
}




static sheet* Setting_HMI_get_focus(Setting_HMI *self, int arg)
{
	

//	uint8_t		f = 0;
	
//	if(arg < 0) {
//		
//		f = self->f_col;
//	} else {
//		f = arg;
//		
//	}
//	
//	if(f == 0)
//		return g_p_ico_memu;
//	else if(f == 1)
//	{
//		if(self->sub_flag & HAS_PGUP)
//			return g_p_ico_pgup;
//		else
//			return g_p_ico_pgdn;
//		
//	}
//	else if(f == 2)
//	{
//		return g_p_ico_pgdn;
//		
//	}
	return NULL;
}

static void Strategy_focus_text(Setting_HMI *self, strategy_focus_t *p_syf, int opt)
{
	int f_data_len = 0;
	uint16_t	txt_xsize, txt_ysize;
	self->p_sht_text->p_gp->getSize(self->p_sht_text->p_gp, self->p_sht_text->cnt.font, &txt_xsize, &txt_ysize);
	f_data_len = self->p_sy->get_focus_data(&self->p_sht_text->cnt.data, p_syf);
	if(f_data_len <= 0)
		return;
	if(opt == 1)		//特效文字
		self->p_sht_text->cnt.colour = COLOUR_BLUE;
	else	{		//去除特效
		self->p_sht_text->cnt.colour = COLOUR_WHITE;
		if(opt == 2)
			f_data_len = strlen(self->p_sht_text->cnt.data);
	}
	
	self->p_sht_text->cnt.len = f_data_len;
	self->p_sht_text->area.x0 = self->col_vx0[p_syf->f_col] + txt_xsize * p_syf->start_byte;
	self->p_sht_text->area.y0 = Stripe_vy(p_syf->f_row - self->entry_start_row);
	self->p_sht_text->p_gp->vdraw(self->p_sht_text->p_gp, &self->p_sht_text->cnt, &self->p_sht_text->area);
	return;
	
}

static void Strategy_focus(Setting_HMI *self, strategy_focus_t *p_syf, int opt)
{
//	int f_data_len = 0;
	uint16_t	txt_xsize, txt_ysize;
	self->p_sht_text->p_gp->getSize(self->p_sht_text->p_gp, self->p_sht_text->cnt.font, &txt_xsize, &txt_ysize);
	//180109 刷新一下焦点的长度，因为焦点的数据可能被修改
	self->p_sy->get_focus_data(&self->p_sht_text->cnt.data, p_syf);		
	if(opt == 2) {
		//擦除整行
		self->p_sht_clean->area.x0 = self->col_vx0[p_syf->f_col] + txt_xsize * p_syf->start_byte - 4;
//		self->p_sht_clean->area.x1 = self->p_sht_CUR->area.x0 + txt_xsize * ( strlen(self->p_sht_text->cnt.data) + 1) + 4;
		self->p_sht_clean->area.x1 = self->p_sht_CUR->area.x0 + txt_xsize * ( 20) + 4;
		self->p_sht_clean->area.y0 = Stripe_vy(p_syf->f_row - self->entry_start_row) -1;
		self->p_sht_clean->area.y1 = self->p_sht_clean->area.y0 + txt_ysize + 2;
		self->p_sht_clean->p_gp->vdraw(self->p_sht_clean->p_gp, &self->p_sht_clean->cnt, &self->p_sht_clean->area);

		return;
	}
	
	if(opt == 0) {
		//擦除光标
//		self->p_sht_CUR->cnt.colour = Stripe_clean_clr(p_syf->f_row);
		
		self->p_sht_clean->area.x0 = self->col_vx0[p_syf->f_col] + txt_xsize * p_syf->start_byte - 4;
		self->p_sht_clean->area.x1 = self->p_sht_CUR->area.x0 + txt_xsize * p_syf->num_byte  + 4;
		self->p_sht_clean->area.y0 = Stripe_vy(p_syf->f_row - self->entry_start_row) -1;
		self->p_sht_clean->area.y1 = self->p_sht_clean->area.y0 + txt_ysize + 2;
		self->p_sht_clean->p_gp->vdraw(self->p_sht_clean->p_gp, &self->p_sht_clean->cnt, &self->p_sht_clean->area);
	} else if(opt == 1){
//		self->p_sht_CUR->cnt.colour = STRIPE_CLR_FOCUSE;
		//显示光标
		self->p_sht_CUR->area.x0 = self->col_vx0[p_syf->f_col] + txt_xsize * p_syf->start_byte - 3;
		self->p_sht_CUR->area.x1 = self->p_sht_CUR->area.x0 + txt_xsize * p_syf->num_byte  + 3;
		self->p_sht_CUR->area.y0 = Stripe_vy(p_syf->f_row - self->entry_start_row) - 1;
		self->p_sht_CUR->area.y1 = self->p_sht_CUR->area.y0 + txt_ysize + 1;
		self->p_sht_CUR->p_gp->vdraw(self->p_sht_CUR->p_gp, &self->p_sht_CUR->cnt, &self->p_sht_CUR->area);
	}
	Strategy_focus_text(self, p_syf, opt);

	
	//恢复颜色
	self->p_sht_text->cnt.colour = COLOUR_WHITE;
}

static void	Setting_HMI_hitHandle(HMI *self, char *s_key)
{
	
	Setting_HMI				*cthis = SUB_PTR( self, HMI, Setting_HMI);
	strategy_t				*p_sy = cthis->p_sy;
	strategy_focus_t	old_sf;
	Button						*p = BTN_Get_Sington();
	
//	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = cthis->f_col;
	uint8_t		sy_chgFouse = 0;
//	uint8_t		chgFouse = 0;

	old_sf.f_col = p_sy->sf.f_col;
	old_sf.f_row = p_sy->sf.f_row;
	old_sf.start_byte = p_sy->sf.start_byte;
	old_sf.num_byte = p_sy->sf.num_byte;
	
	if( !strcmp( s_key, HMIKEY_LEFT) )
	{
		
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_lt(NULL) == RET_OK) {
			
				sy_chgFouse = 1;
			} else {
				sy_chgFouse = 2;
				CLR_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
				
				
			}
		}
		if((cthis->sub_flag & FOCUS_IN_STARTEGY) == 0) {
			self->btn_backward(self);
//			chgFouse = 1;
		}
		
	}
	
	if( !strcmp( s_key, HMIKEY_RIGHT) )
	{
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_rt(NULL) == RET_OK) {
			
				sy_chgFouse = 1;
			}  else {
				sy_chgFouse = 2;
				CLR_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
				
				
			}
		}

		if((cthis->sub_flag & FOCUS_IN_STARTEGY) == 0) {
			self->btn_forward(self);
//			chgFouse = 1;
		}
		
	}
	if( !strcmp( s_key, HMIKEY_UP) )
	{
//		if(phn_sys.key_weight == 0)
			phn_sys.key_weight = 1;
		if(phn_sys.hit_count < 10)
			phn_sys.hit_count ++;
		else
			phn_sys.hit_count = 0;
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_up(NULL) == RET_OK) {
				//编辑区内的上下操作是用来修改参数的
				//因此在修改参数成功之后，要重新显示被选中区的值
				sy_chgFouse = 3;
			} 
		} else {
			
			//在界面中，只有一行，因此任何的行切换操作，都意味将光标切到编辑区内
			//吧光标切回编辑区之前，先将编辑区之外的光标清除掉
			self->clear_focus(self, 0, focusCol);
			Strategy_focus(cthis, &cthis->p_sy->sf, 1);
			SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
		}
		
	}
	
	if( !strcmp( s_key, HMIKEY_DOWN) )
	{
//		if(phn_sys.key_weight == 0)
			phn_sys.key_weight = 1;
//		phn_sys.hit_count = 0;
		if(phn_sys.hit_count < 10)
			phn_sys.hit_count ++;
		else
			phn_sys.hit_count = 0;
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_dn(NULL) == RET_OK) {
				
				sy_chgFouse = 3;
			} 
		} else {
			
			//在界面中，只有一行，因此任何的行切换操作，都意味将光标切到编辑区内
			//吧光标切回编辑区之前，先将编辑区之外的光标清除掉
			self->clear_focus(self, 0, focusCol);
			Strategy_focus(cthis, &cthis->p_sy->sf, 1);
			SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
		}
	}
	
	
	if( !strcmp(s_key, HMIKEY_ENTER))
	{	
		
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			cthis->p_sy->key_hit_er(NULL) ;
			
		} else {
			if(self->flag & HMIFLAG_FOCUS_IN_BTN)
			{
				p->hit();
//				self->btn_hit(self);
			}
//			p_focus = Setting_HMI_get_focus(cthis, -1);
//			if(STING_Show_Button(self, p_focus->id) == ERR_OPT_FAILED) {
//				if(p_focus->id == ICO_ID_MENU)
//				{
//					cthis->entry_start_row = 0;
//					self->switchHMI(self, g_p_HMI_menu);
//				}
//				
//			}
		}
	}
	
	if( !strcmp(s_key, HMIKEY_ESC))
	{
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			//按esc键跳出编辑区
			
			sy_chgFouse = 2;
			CLR_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
			
			
			cthis->f_col = 0;
		} else {
			
			cthis->entry_start_row = 0;
			self->switchHMI(self, g_p_Setup_HMI);
			
		}
		
	}
	
	if(sy_chgFouse == 1) {
		Strategy_focus(cthis, &old_sf, 0);
		Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	} else if(sy_chgFouse == 2){
		//光标从编辑区跳出
		Strategy_focus(cthis, &old_sf, 0);
//		self->show_focus(self, 0, 0);
	}
	else if(sy_chgFouse ==3){
		//擦除掉原来这一行
		Strategy_focus(cthis, &old_sf, 2);
		//重新显示改行文本
//		Strategy_focus_text(cthis, &cthis->p_sy->sf, 2);
		//显示新的选中效果
		Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	}
	
//	if( chgFouse) {	
//		self->clear_focus(self, 0, focusCol);
//		self->show_focus(self, 0, 0);
//		
//	}
	
//	exit:
		return;
}




static void	Setting_HMI_long_hit( HMI *self, char *s_key)
{
	Setting_HMI			*cthis = SUB_PTR( self, HMI, Setting_HMI);
	strategy_keyval_t	skt = {SY_KEYTYPE_LONGPUSH};
	strategy_t			*p_sy = cthis->p_sy;
	



	
	if((cthis->sub_flag & FOCUS_IN_STARTEGY) == 0)
		return;
	if(phn_sys.hit_count == 10)
	{
		if(phn_sys.key_weight < 10000)
			phn_sys.key_weight *= 10;
//		else
//			phn_sys.key_weight = 1;
		phn_sys.hit_count = 0;
	}
	else
	{
		phn_sys.hit_count ++;
//		return;
	}
	
	if( !strcmp( s_key, HMIKEY_UP) )
	{
		p_sy->key_hit_up(&skt);
		
		
	}
	
	if( !strcmp( s_key, HMIKEY_DOWN) )
	{
		p_sy->key_hit_dn(&skt);
		
	}
	
	

	//显示新的选中效果
	Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	//180114 长按的时候尽快刷新
	Flush_LCD();
}
//长按按键:up,dn 来对光标的行进行切换
//切换有两种情况:编辑区内的行切换, 编辑区内外之间的切换
static void	Setting_HMI_dhit( HMI *self, char *s_key)
{
//	Setting_HMI			*cthis = SUB_PTR( self, HMI, Setting_HMI);
//	strategy_keyval_t	skt = {SY_KEYTYPE_LONGPUSH};
//	strategy_t			*p_sy = cthis->p_sy;
//	strategy_focus_t	old_sf;
//	


//	old_sf.f_col = p_sy->sf.f_col;
//	old_sf.f_row = p_sy->sf.f_row;
//	old_sf.start_byte = p_sy->sf.start_byte;
//	old_sf.num_byte = p_sy->sf.num_byte;
//	
//	if((cthis->sub_flag & FOCUS_IN_STARTEGY) == 0)
//		return;
//	if(strcmp( s_key, HMIKEY_UP) && strcmp( s_key, HMIKEY_DOWN))
//		return;
//	
//	
//	if(phn_sys.key_weight < 10000)
//		phn_sys.key_weight *= 10;
//	else
//		phn_sys.key_weight = 1;
//	
//	if( !strcmp( s_key, HMIKEY_UP) )
//	{
//		p_sy->key_hit_up(&skt);
//		
//		
//	}
//	
//	if( !strcmp( s_key, HMIKEY_DOWN) )
//	{
//		p_sy->key_hit_dn(&skt);
//		
//	}
//	
//	
//	Strategy_focus(cthis, &old_sf, 2);
//	//重新显示改行文本
//	Strategy_focus_text(cthis, &cthis->p_sy->sf, 2);
//	//显示新的选中效果
//	Strategy_focus(cthis, &cthis->p_sy->sf, 1);	
}
static void Clean_stripe(HMI *self)
{
	//todo:cpic 运行之后的结果好像不对
//	g_p_cpic->e_heifht = 1;
//	Sheet_slide(g_p_cpic);
//	g_p_cpic->e_heifht = 0;
	Sheet_refresh(g_p_sht_bkpic);
}

//切换页面时，将光标重新至于编辑区
static int STING_Show_Button(HMI *self, int up_or_dn)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	if(up_or_dn == ICO_ID_PGDN) {
		cthis->entry_start_row += STRIPE_MAX_ROWS;
	} else if(up_or_dn == ICO_ID_PGUP) {
		cthis->entry_start_row -= STRIPE_MAX_ROWS;
	} else {
		return ERR_OPT_FAILED;
	}

	Clean_stripe(self);
	Show_entry(self, cthis->p_sy);
	SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
	
//	g_p_ico_pgup->cnt.effects = GP_CLR_EFF(g_p_ico_pgup->cnt.effects, EFF_FOCUS);
//	g_p_ico_pgdn->cnt.effects = GP_CLR_EFF(g_p_ico_pgdn->cnt.effects, EFF_FOCUS);
//	g_p_ico_memu->cnt.effects = GP_CLR_EFF(g_p_ico_memu->cnt.effects, EFF_FOCUS);
//	self->show_focus(self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
	self->clear_focus(self, 0-1, -1);
	Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	Flush_LCD();
	
	return RET_OK;
}

static void	Show_entry(HMI *self, strategy_t *p_st)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	char					*p_trash;
	Button				*p_btn = BTN_Get_Sington();
	uint8_t	row = 0;
	uint8_t	col = 0;

	uint8_t	text_len = 0;
	uint8_t	col_maxlen = 0;			//某一列的列宽，取最大的那个列宽
	
	uint16_t	txt_xsize = 0;
	uint16_t	txt_ysize = 0;
	uint8_t		col_vx0 = 0;
	uint8_t		col_space = 2;				//列间距：单位 字符的宽度
	uint8_t		more = 0;					//在本页显示完之后，是否还有内容需要显示
	
	if(p_st == NULL)
		return;
	
	cthis->p_sht_text->p_gp->getSize(cthis->p_sht_text->p_gp, cthis->p_sht_text->cnt.font, &txt_xsize, &txt_ysize);
	
	for(col = 0; col < 3; col ++) {
		
		col_vx0 += col_maxlen * txt_xsize;
		cthis->col_vx0[col] = col_vx0;
		//显示一页中所有的行
		col_maxlen = 0;
		for(row = 0; row < STRIPE_MAX_ROWS; row ++) {
			
			text_len = p_st->entry_txt(row + cthis->entry_start_row, col, &cthis->p_sht_text->cnt.data);
			if(text_len == 0)
				continue;
			cthis->p_sht_text->cnt.len = text_len;
			if(col_maxlen < text_len)
				col_maxlen = text_len;
			
			cthis->p_sht_text->area.x0 = col_vx0;
			cthis->p_sht_text->area.y0 = Stripe_vy(row);
//			if(row == 0)
//				cthis->p_sht_text->area.y0 = STRIPE_VY0 ;
//			else 
//				cthis->p_sht_text->area.y0 = STRIPE_VY1 + (row - 1)* STRIPE_SIZE_Y;
			
			cthis->p_sht_text->p_gp->vdraw(cthis->p_sht_text->p_gp, &cthis->p_sht_text->cnt, &cthis->p_sht_text->area);
			
		}
		//检查是否还有内容需要显示
			//通过对本页的已经显示完的下一行进行检测来判断
			
		text_len = p_st->entry_txt(row + cthis->entry_start_row, col, &p_trash);
		if(text_len > 0)
			more = 1;		//任何一列需要更多的显示，就认为需要更多的显示
		
		col_maxlen += col_space;
	}
	
	if(cthis->entry_start_row) {
//		g_p_ico_pgup->e_heifht = 1;
//		Sheet_slide(g_p_ico_pgup);
//		g_p_ico_pgup->e_heifht = 0;
//		SET_PG_FLAG(cthis->sub_flag, HAS_PGUP);
		
		p_btn->build_each_btn(1, BTN_TYPE_PGUP, Setting_btn_hdl, self);
		p_btn->show_vaild_btn();
	} 
//	else {
//		CLR_PG_FLAG(cthis->sub_flag, HAS_PGUP);
//	}
	
	if(more) 
	{
		p_btn->build_each_btn(1, BTN_TYPE_PGDN, Setting_btn_hdl, self);
		p_btn->show_vaild_btn();
//		g_p_ico_pgdn->e_heifht = 1;
//		Sheet_slide(g_p_ico_pgdn);
//		g_p_ico_pgdn->e_heifht = 0;
//		
//		SET_PG_FLAG(cthis->sub_flag, HAS_PGDN);
	} 
	
//	else {
//		CLR_PG_FLAG(cthis->sub_flag, HAS_PGDN);
//	}
	
//	if((cthis->sub_flag & 0x0f)== 0)
//		cthis->col_max = 1;
//	if(cthis->sub_flag & HAS_PGUP)
//		cthis->col_max = 2;
//	if(cthis->sub_flag & HAS_PGDN)
//		cthis->col_max = 2;

	
}


static int Setting_Sy_cmd(void *p_rcv, int cmd,  void *arg)
{
	HMI								*self = (HMI *)p_rcv;
	Setting_HMI				*cthis = SUB_PTR( self, HMI, Setting_HMI);
	winHmi						*p_win;
	strategy_focus_t	*p_pos;
	int 							ret = RET_OK;
	char							win_tips[32];
	
	switch(cmd) {
		case sycmd_reflush:
			cthis->p_sht_text->cnt.colour = COLOUR_WHITE;
//			Show_entry(self, cthis->p_sy);
			self->show(self);
			self->show_button(self);
//			Strategy_focus(cthis, &cthis->p_sy->sf, 1);
//			Flush_LCD();
			break;
		case sycmd_reflush_position:
			p_pos = (strategy_focus_t		*)arg;
		
			cthis->p_sht_text->cnt.len = \
			cthis->p_sy->entry_txt(p_pos->f_row, p_pos->f_col, &cthis->p_sht_text->cnt.data);
			if(cthis->p_sht_text->cnt.len == 0)
				break;
			cthis->p_sht_text->cnt.colour = COLOUR_WHITE;
			cthis->p_sht_text->area.x0 = cthis->col_vx0[p_pos->f_col];
			cthis->p_sht_text->area.y0 = Stripe_vy(p_pos->f_row);
			
			cthis->p_sht_text->p_gp->vdraw(cthis->p_sht_text->p_gp, &cthis->p_sht_text->cnt, &cthis->p_sht_text->area);
		
		
			break;
		case sycmd_win_tips:
			
			g_p_winHmi->arg[0] = WINTYPE_TIPS;
			g_p_winHmi->arg[1] |= WINFLAG_RETURN | WINFLAG_COMMIT;
			p_win = Get_winHmi();
			p_win->p_cmd_rcv = self;
			p_win->cmd_hdl = Setting_Sy_cmd;
			self->switchHMI(self, g_p_winHmi);
			break;
		case sycmd_win_time:
			Win_content(arg);
			g_p_winHmi->arg[0] = WINTYPE_TIME_SET;
			g_p_winHmi->arg[1] = 0;
			p_win = Get_winHmi();
			p_win->p_cmd_rcv = self;
			p_win->cmd_hdl = Setting_Sy_cmd;
			self->switchHMI(self, g_p_winHmi);
			break;
		case sycmd_win_psd:
			Win_content(arg);
			g_p_winHmi->arg[0] = WINTYPE_PASSWORD_SET;
			g_p_winHmi->arg[1] = 0;
			p_win = Get_winHmi();
			p_win->p_cmd_rcv = self;
			p_win->cmd_hdl = Setting_Sy_cmd;
			self->switchHMI(self, g_p_winHmi);
			break;
		case sycmd_keyboard:
			cthis->p_sht_text->cnt.data = arg;
			g_keyHmi->p_shtInput = cthis->p_sht_text;
			self->switchHMI(self, SUPER_PTR(g_keyHmi, HMI));
			break;
		case wincmd_commit:
			
			ret = cthis->p_sy->commit(NULL) ;
			if(ret == RET_OK) {
				g_p_winHmi->arg[0] = WINTYPE_TIPS;
				g_p_winHmi->arg[1] = WINFLAG_RETURN;
				Win_content("修改成功");
//				g_p_winHmi->switchHMI(g_p_winHmi, g_p_winHmi);
				self->switchHMI(self, g_p_winHmi);
			} else {
				g_p_winHmi->arg[0] = WINTYPE_ERROR;
				g_p_winHmi->arg[1] = WINFLAG_RETURN;
				sprintf(win_tips,"错误码:%d", ret);
				Win_content(win_tips);
//				g_p_winHmi->switchHMI(g_p_winHmi, g_p_winHmi);
				self->switchHMI(self, g_p_winHmi);
//				g_p_winHmi->show(g_p_winHmi);
			}
		
			break;
		
	}
	
	return ret;
	
	
}

