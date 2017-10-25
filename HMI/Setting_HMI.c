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
#define SETTING_TITLE		"����"

static const char setting_hmi_code_clean[] =  {"<cpic vx0=0 vy0=32 vx1=320 vy1=240>16</>" };
static const char setting_hmi_code_text[] =  {"<text  f=16 clr=white m=0> </>" };
static const char setting_hmi_code_CUR[] =  {"<line > </>" };



static char *setting_titles[4][2] = {{"ϵͳ����", "ͨ������"},{"��������", "�㷨����"},\
{"��ʾ����", "���ݱ���"},{"���ݴ�ӡ", "�˳�"}};
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
 {&g_view_strategy, &g_DBU_strategy},{&g_dataPrint_strategy, NULL}};
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
static void	Setting_HMI_longpush( HMI *self, char *s_key);


static void	Show_entry(HMI *self, strategy_t *p_st);
static int Show_more(HMI *self, int up_or_dn);
 
 
static sheet* Setting_HMI_get_focus(Setting_HMI *self, int arg);
static void Strategy_focus(Setting_HMI *self, strategy_focus_t *p_syf, int opt);
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

CTOR(Setting_HMI)
SUPER_CTOR( HMI);
FUNCTION_SETTING(HMI.init, Init_Setting_HMI);
FUNCTION_SETTING(HMI.initSheet, Setting_initSheet);
FUNCTION_SETTING(HMI.hide, Setting_HMI_hide);
FUNCTION_SETTING(HMI.show, Show_Setting_HMI);

FUNCTION_SETTING(HMI.hitHandle, Setting_HMI_hitHandle);
FUNCTION_SETTING(HMI.longpushHandle, Setting_HMI_longpush);

FUNCTION_SETTING(HMI.init_focus, Setting_HMI_init_focus);
FUNCTION_SETTING(HMI.clear_focus, Setting_HMI_clear_focus);
FUNCTION_SETTING(HMI.show_focus, Setting_HMI_show_focus);


END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_Setting_HMI(HMI *self, void *arg)
{
//	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
//	short				i = 0;	
//	p_shtctl = GetShtctl();
//	

//	p_exp = ExpCreate( "pic");
	
		
	return RET_OK;
}

static void Show_Setting_HMI(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	int ret = 0;
	char		win_tips[32];
	
	
	if(self->flag & HMIFLAG_WIN) {
		if((self->arg[1] == 0) &&  (cthis->sub_flag & DO_NOTHING) == 0) { 		//���ڻ��洫�ݹ����ļ����λ��Ϊ��ȷ����
			//Ȼ�����ѡ����д���
			ret = cthis->p_sy->key_hit_er(NULL) ;
			if(ret == RET_OK) {
				SET_PG_FLAG(cthis->sub_flag, DO_NOTHING);		//�´δӴ��ڽ����лص�ʱ�򣬾Ͳ�������
				g_p_winHmi->arg[0] = WINTYPE_CUR;
				Win_SetTips("�޸ĳɹ�");
				self->switchHMI(self, g_p_winHmi);
			} else {
				SET_PG_FLAG(cthis->sub_flag, DO_NOTHING);		//
				g_p_winHmi->arg[0] = WINTYPE_ERROR;
				sprintf(win_tips,"������:%d", ret);
				Win_SetTips(win_tips);
				self->switchHMI(self, g_p_winHmi);
			}
			
			return;
			
		}
		
	} 
	Sheet_refresh(g_p_sht_bkpic);
	cthis->entry_start_row = 0;
	Show_entry(self, cthis->p_sy);
	Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	
	
}
static void	Setting_initSheet(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	int  		 			h = 0;
	Expr 					*p_exp ;
	shtctl 				*p_shtctl = NULL;
	p_shtctl = GetShtctl();
	
	
	if((self->flag & HMIFLAG_WIN) == 0) 
		cthis->p_sy = arr_p_setting_strategy[self->arg[0]][self->arg[1]];
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_clean, g_p_cpic) ;
	
	p_exp = ExpCreate( "text");
	cthis->p_sht_text = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_text, cthis->p_sht_text) ;
	
	p_exp = ExpCreate( "line");
	cthis->p_sht_CUR = Sheet_alloc(p_shtctl);
	p_exp->inptSht( p_exp, (void *)setting_hmi_code_CUR, cthis->p_sht_CUR) ;
	


	g_p_sht_bkpic->cnt.data = SETTING_PICNUM;

	
	g_p_sht_title->cnt.data = setting_titles[self->arg[0]][self->arg[1]];
	g_p_sht_title->cnt.len = strlen(g_p_sht_title->cnt.data);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	Sheet_updown(g_p_ico_memu, h++);
	

	self->init_focus(self);
}
static void	Setting_HMI_hide(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);

	Sheet_updown(g_p_ico_memu, -1);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	
	Sheet_free(cthis->p_sht_text);
	Sheet_free(cthis->p_sht_CUR);
}


static void	Setting_HMI_init_focus(HMI *self)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	cthis->f_col = 0;
	cthis->sub_flag &= 0xf0;
	cthis->col_max = 1;
	cthis->p_sy->init(NULL);
	SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
	
//	self->p_fcuu = Focus_alloc(1, 2);
//	Focus_Set_sht(self->p_fcuu, 0, 0, g_p_ico_pgup);
//	Focus_Set_sht(self->p_fcuu, 0, 0, g_p_ico_pgdn);	
//	Focus_Set_focus(self->p_fcuu, 0, 2);
}

static void	Setting_HMI_clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	sheet *p_fouse = Setting_HMI_get_focus(cthis, fouse_col);
	
	if(p_fouse == NULL)
		return;
	p_fouse->cnt.effects = GP_CLR_EFF(p_fouse->cnt.effects, EFF_FOCUS);
	
	p_fouse->e_heifht = 1;
	Sheet_slide(p_fouse);
	p_fouse->e_heifht = 0;
	

}
static void	Setting_HMI_show_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	sheet *p_fouse = Setting_HMI_get_focus(cthis, -1);
	
	if(p_fouse == NULL)
		return;
	p_fouse->cnt.effects = GP_SET_EFF(p_fouse->cnt.effects, EFF_FOCUS);
	p_fouse->e_heifht = 1;
	Sheet_slide(p_fouse);
	p_fouse->e_heifht = 0;
}

static void Setting_HMI_add_focus(Setting_HMI *self)
{
	if(self->f_col < (self->col_max - 1))
		self->f_col ++;
	else
		self->f_col = 0;
	
}
static void Setting_HMI_minus_focus(Setting_HMI *self)
{
	if(self->f_col > 0)
		self->f_col --;
	else
		self->f_col = self->col_max - 1;
	
}

static sheet* Setting_HMI_get_focus(Setting_HMI *self, int arg)
{
	uint8_t		f = 0;
	
	if(arg < 0) {
		
		f = self->f_col;
	} else {
		f = arg;
		
	}
	
	if(f == 0)
		return g_p_ico_memu;
	else if(f == 1)
	{
		if(self->sub_flag & HAS_PGUP)
			return g_p_ico_pgup;
		else
			return g_p_ico_pgdn;
		
	}
	else if(f == 2)
	{
		return g_p_ico_pgdn;
		
	}
	return NULL;
}

static void Strategy_focus(Setting_HMI *self, strategy_focus_t *p_syf, int opt)
{
	int f_data_len = 0;
	uint16_t	txt_xsize, txt_ysize;
	self->p_sht_text->p_gp->getSize(self->p_sht_text->p_gp, self->p_sht_text->cnt.font, &txt_xsize, &txt_ysize);
	
	if(opt == 2) {
		//����ˢ��ѡ�������ֵ
		f_data_len = self->p_sy->get_focus_data(&self->p_sht_text->cnt.data);
		if(f_data_len <= 0)
			return;
		self->p_sht_text->cnt.len = f_data_len;
		self->p_sht_text->area.x0 = self->col_vx0[p_syf->f_col] + txt_xsize * p_syf->start_byte;
		self->p_sht_text->area.y0 = Stripe_vy(p_syf->f_row);
		self->p_sht_text->p_gp->vdraw(self->p_sht_text->p_gp, &self->p_sht_text->cnt, &self->p_sht_text->area);
		return;
	}
	
	if(opt == 0) {
		self->p_sht_CUR->cnt.colour = Stripe_clean_clr(p_syf->f_row);
	} else if(opt == 1){
		
		self->p_sht_CUR->cnt.colour = STRIPE_CLR_FOCUSE;
	}
	
//	txt_xsize = txt_xsize >> 1;		//Ӣ�Ļ������ǰ����С
	self->p_sht_CUR->area.x0 = self->col_vx0[p_syf->f_col] + txt_xsize * p_syf->start_byte;
	self->p_sht_CUR->area.x1 = self->p_sht_CUR->area.x0 + txt_xsize * p_syf->num_byte;
	self->p_sht_CUR->area.y0 = Stripe_vy(p_syf->f_row) + txt_ysize - 2;
	self->p_sht_CUR->area.y1 = self->p_sht_CUR->area.y0;
	
	self->p_sht_CUR->p_gp->vdraw(self->p_sht_CUR->p_gp, &self->p_sht_CUR->cnt, &self->p_sht_CUR->area);
	self->p_sht_CUR->area.y0 = Stripe_vy(p_syf->f_row) + txt_ysize - 1;
	self->p_sht_CUR->area.y1 = self->p_sht_CUR->area.y0;
	self->p_sht_CUR->p_gp->vdraw(self->p_sht_CUR->p_gp, &self->p_sht_CUR->cnt, &self->p_sht_CUR->area);
	
}

static void	Setting_HMI_hitHandle(HMI *self, char *s_key)
{
	
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	sheet			*p_focus;
	strategy_t			*p_sy = cthis->p_sy;
	strategy_focus_t	old_sf;
	
	
//	uint8_t		focusRow = self->p_fcuu->focus_row;
	uint8_t		focusCol = cthis->f_col;
	uint8_t		sy_chgFouse = 0;
	uint8_t		chgFouse = 0;

	old_sf.f_col = p_sy->sf.f_col;
	old_sf.f_row = p_sy->sf.f_row;
	old_sf.start_byte = p_sy->sf.start_byte;
	old_sf.num_byte = p_sy->sf.num_byte;
	
	if( !strcmp( s_key, HMIKEY_LEFT) )
	{
		
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_lt(NULL) == RET_OK) {
			
				sy_chgFouse = 1;
			} 
		} else {
			
			Setting_HMI_minus_focus(cthis);
			chgFouse = 1;
		}
		
	}
	
	if( !strcmp( s_key, HMIKEY_RIGHT) )
	{
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_rt(NULL) == RET_OK) {
			
				sy_chgFouse = 1;
			} 
		} else {
			
			Setting_HMI_add_focus(cthis);
			chgFouse = 1;
		}
		
	}
	if( !strcmp( s_key, HMIKEY_UP) )
	{
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_up(NULL) == RET_OK) {
				//�༭���ڵ����²����������޸Ĳ�����
				//������޸Ĳ����ɹ�֮��Ҫ������ʾ��ѡ������ֵ
				sy_chgFouse = 3;
			} 
		} 
		
	}
	
	if( !strcmp( s_key, HMIKEY_DOWN) )
	{
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(p_sy->key_hit_dn(NULL) == RET_OK) {
				
				sy_chgFouse = 3;
			} 
		} 
	}
	
	
	if( !strcmp(s_key, HMIKEY_ENTER))
	{	
		
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			//��ʾ
			g_p_winHmi->arg[0] = WINTYPE_CUR;
			Win_SetTips("ȷ���޸ģ�");
			self->switchHMI(self, g_p_winHmi);
			cthis->sub_flag &= ~DO_NOTHING;
		} else {
			p_focus = Setting_HMI_get_focus(cthis, -1);
			if(Show_more(self, p_focus->id) == ERR_OPT_FAILED) {
				if(p_focus->id == ICO_ID_MENU)
					self->switchHMI(self, g_p_HMI_menu);
				
			}
		}
	}
	
	if( !strcmp(s_key, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
	if(sy_chgFouse == 1) {
		Strategy_focus(cthis, &old_sf, 0);
		Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	} else if(sy_chgFouse == 2){
		//���ӱ༭������
		Strategy_focus(cthis, &cthis->p_sy->sf, 0);
		self->show_focus(self, 0, 0);
	} else if(sy_chgFouse ==3){
		//ˢ��ѡ�������ֵ
		Strategy_focus(cthis, &cthis->p_sy->sf, 2);
	}else if( chgFouse) {	
		self->clear_focus(self, 0, focusCol);
		self->show_focus(self, 0, 0);
		
	}
	
//	exit:
		return;
}

//��������:up,dn ���Թ����н����л�
//�л����������:�༭���ڵ����л�, �༭������֮����л�
static void	Setting_HMI_longpush( HMI *self, char *s_key)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	strategy_keyval_t	skt = {SY_KEYTYPE_LONGPUSH};
	strategy_focus_t	old_sf;
	uint8_t		focusCol = cthis->f_col;
	uint8_t		sy_chgFouse = 0;

	old_sf.f_col = cthis->p_sy->sf.f_col;
	old_sf.f_row = cthis->p_sy->sf.f_row;
	old_sf.start_byte = cthis->p_sy->sf.start_byte;
	old_sf.num_byte =cthis->p_sy->sf.num_byte;
	
	
	if( !strcmp( s_key, HMIKEY_UP) )
	{
		
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			//���λ�ڱ༭�������ñ༭���Ĵ�����������
			
			if(cthis->p_sy->key_hit_up(&skt) == RET_OK) {
				
				sy_chgFouse = 1;
			} else {
				//����ڱ༭���ڲ��л������˴�������Ϊ���Ѿ������˱༭���ķ�Χ�ˣ�������л����༭����
				sy_chgFouse = 2;
				
				CLR_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
				
			}  
		} else {
			//�ڽ����У�ֻ��һ�У�����κε����л�����������ζ������е��༭����
			//�ɹ���лر༭��֮ǰ���Ƚ��༭��֮��Ĺ�������
			self->clear_focus(self, 0, focusCol);
			Strategy_focus(cthis, &cthis->p_sy->sf, 1);
			SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
			
		}
		
	}
	
	if( !strcmp( s_key, HMIKEY_DOWN) )
	{
		if(cthis->sub_flag & FOCUS_IN_STARTEGY) {
			
			if(cthis->p_sy->key_hit_dn(&skt) == RET_OK) {
				
				sy_chgFouse = 1;
			} else {
				sy_chgFouse = 2;
				CLR_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
				
			}  
		} else {
			self->clear_focus(self, 0, focusCol);
			Strategy_focus(cthis, &cthis->p_sy->sf, 1);
			SET_PG_FLAG(cthis->sub_flag, FOCUS_IN_STARTEGY);
			
		}
	}
	if(sy_chgFouse == 1) {
		Strategy_focus(cthis, &old_sf, 0);
		Strategy_focus(cthis, &cthis->p_sy->sf, 1);
	} else if(sy_chgFouse == 2){
		//���ӱ༭������
		Strategy_focus(cthis, &cthis->p_sy->sf, 0);
		self->show_focus(self, 0, 0);
	}
	
}
static void Clean_stripe(HMI *self)
{
	//todo:cpic ����֮��Ľ�����񲻶�
//	g_p_cpic->e_heifht = 1;
//	Sheet_slide(g_p_cpic);
//	g_p_cpic->e_heifht = 0;
	Sheet_refresh(g_p_sht_bkpic);
}

static int Show_more(HMI *self, int up_or_dn)
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
	Flush_LCD();
	
	return RET_OK;
}

static void	Show_entry(HMI *self, strategy_t *p_st)
{
	Setting_HMI		*cthis = SUB_PTR( self, HMI, Setting_HMI);
	char			*p_trash;
	uint8_t	row = 0;
	uint8_t	col = 0;

	uint8_t	text_len = 0;
	uint8_t	col_maxlen = 0;			//ĳһ�е��п���ȡ�����Ǹ��п�
	
	uint16_t	txt_xsize = 0;
	uint16_t	txt_ysize = 0;
	uint8_t		col_vx0 = 0;
	uint8_t		col_space = 1;				//�м�ࣺ��λ �ַ��Ŀ���
	uint8_t		more = 0;					//�ڱ�ҳ��ʾ��֮���Ƿ���������Ҫ��ʾ
	
	if(p_st == NULL)
		return;
	
	cthis->p_sht_text->p_gp->getSize(cthis->p_sht_text->p_gp, cthis->p_sht_text->cnt.font, &txt_xsize, &txt_ysize);
	
	for(col = 0; col < 2; col ++) {
		
		col_vx0 = col_maxlen * txt_xsize;
		cthis->col_vx0[col] = col_vx0;
		//��ʾһҳ�����е���
		for(row = 0; row < STRIPE_MAX_ROWS; row ++) {
			
			text_len = p_st->entry_txt(row + cthis->entry_start_row, col, &cthis->p_sht_text->cnt.data);
			if(text_len == 0)
				continue;
			cthis->p_sht_text->cnt.len = text_len;
			if(col_maxlen < text_len)
				col_maxlen = text_len;
			
			cthis->p_sht_text->area.x0 = col_vx0;
			if(row == 0)
				cthis->p_sht_text->area.y0 = STRIPE_VY0 ;
			else 
				cthis->p_sht_text->area.y0 = STRIPE_VY1 + (row - 1)* STRIPE_SIZE_Y;
			
			cthis->p_sht_text->p_gp->vdraw(cthis->p_sht_text->p_gp, &cthis->p_sht_text->cnt, &cthis->p_sht_text->area);
			
		}
		//����Ƿ���������Ҫ��ʾ
			//ͨ���Ա�ҳ���Ѿ���ʾ�����һ�н��м�����ж�
			
		text_len = p_st->entry_txt(row + cthis->entry_start_row, col, &p_trash);
		if(text_len > 0)
			more = 1;		//�κ�һ����Ҫ�������ʾ������Ϊ��Ҫ�������ʾ
		
		col_maxlen += col_space;
	}
	if(cthis->entry_start_row) {
		g_p_ico_pgup->e_heifht = 1;
		Sheet_slide(g_p_ico_pgup);
		g_p_ico_pgup->e_heifht = 0;
		SET_PG_FLAG(cthis->sub_flag, HAS_PGUP);
	} else {
		CLR_PG_FLAG(cthis->sub_flag, HAS_PGUP);
	}
	
	if(more) {
		g_p_ico_pgdn->e_heifht = 1;
		Sheet_slide(g_p_ico_pgdn);
		g_p_ico_pgdn->e_heifht = 0;
		
		SET_PG_FLAG(cthis->sub_flag, HAS_PGDN);
	} else {
		CLR_PG_FLAG(cthis->sub_flag, HAS_PGDN);
	}
	
	if((cthis->sub_flag & 0x0f)== 0)
		cthis->col_max = 1;
	if(cthis->sub_flag & HAS_PGUP)
		cthis->col_max = 2;
	if(cthis->sub_flag & HAS_PGDN)
		cthis->col_max = 2;

	
}

