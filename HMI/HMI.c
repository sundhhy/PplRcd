#include "HMI.h"
#include "HMI_comm.h"
#include <string.h>
#include "os/os_depend.h"
#include "sys_cmd.h"
#include "HMIFactory.h"

//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T Hmi_Failed = { "HMI Failed" };

#define HMI_FLUSH_CYCLE_S				3600
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

	
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
HMI *g_p_curHmi;
HMI *g_p_lastHmi;
HMI *g_p_win_last;

keyboard_commit	kbr_cmt = NULL;
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
static void	HmiShow( HMI *self);
static void	HMI_Run( HMI *self);
static void	SwitchHMI( HMI *self, HMI *p_hmi);
static void	SwitchBack( HMI *self);
static void HitHandle( HMI *self, char *s_key);
static void LngpshHandle( HMI *self, char *s_key);
static void DHitHandle( HMI *self, char *s_key);
static void ConposeKeyHandle(HMI *self, char *s_key1, char *s_key2);

void	Init_focus(HMI *self);
void	Clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
void	Show_focus( HMI *self, uint8_t fouse_row, uint8_t fouse_col);

static void		HMI_Build_cmp(HMI *self);
static void		HMI_Clean_cmp(HMI *self);
static void		HMI_Show_cmp(HMI *self);
static int		HMI_Btn_forward(HMI *self);
static int		HMI_Btn_backward(HMI *self);
static void		HMI_Btn_jumpout(HMI *self);
//static void		HMI_Btn_hit(HMI *self);
static void HMI_Flush(void);		//定期刷屏
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void Set_flag_show(uint8_t	*p_flag, int val)
{
	
	if(val)
		*p_flag |= HMI_FLAG_HIDE;
	else
		*p_flag &= ~HMI_FLAG_HIDE;
//	val &= 1;
//	*p_flag &= 0xfe;
//	*p_flag |= val;
}

int HMI_Init(void)
{
	
	HMI 			*p_mainHmi;
	p_mainHmi = CreateHMI( HMI_MAIN);
	p_mainHmi->init( p_mainHmi, NULL);
	p_mainHmi->switchHMI(p_mainHmi, p_mainHmi);
	Cmd_Rgt_time_task(HMI_Flush, HMI_FLUSH_CYCLE_S);
	
	return RET_OK;
}

//void Set_flag_keyhandle(uint8_t	*p_flag, int val)
//{
//	val &= 2;
//	*p_flag &= 0xfd;
//	*p_flag |= val;
//}






ABS_CTOR( HMI)
FUNCTION_SETTING( show, HmiShow);
FUNCTION_SETTING( hmi_run, HMI_Run);

FUNCTION_SETTING( switchHMI, SwitchHMI);
FUNCTION_SETTING( switchBack, SwitchBack);
FUNCTION_SETTING( hitHandle, HitHandle);
FUNCTION_SETTING( longpushHandle, LngpshHandle);
FUNCTION_SETTING( dhitHandle, DHitHandle);
FUNCTION_SETTING( conposeKeyHandle, ConposeKeyHandle);


FUNCTION_SETTING( init_focus, Init_focus);
FUNCTION_SETTING( clear_focus, Clear_focus);
FUNCTION_SETTING( show_focus, Show_focus);

FUNCTION_SETTING(build_component, HMI_Build_cmp);
FUNCTION_SETTING(clean_cmp, HMI_Clean_cmp);
FUNCTION_SETTING(show_cmp, HMI_Show_cmp);
FUNCTION_SETTING(btn_forward, HMI_Btn_forward);
FUNCTION_SETTING(btn_backward, HMI_Btn_backward);
FUNCTION_SETTING(btn_jumpout, HMI_Btn_jumpout);
//FUNCTION_SETTING(btn_hit, HMI_Btn_hit);

END_ABS_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static void HMI_Flush(void)
{
	Cmd_Rgt_time_task(HMI_Flush, HMI_FLUSH_CYCLE_S);
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	g_p_curHmi->show(g_p_curHmi);
	g_p_curHmi->show_cmp(g_p_curHmi);
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
}


static void	HmiShow( HMI *self)
{
	
	
}

static void	HMI_Run( HMI *self)
{
	
	
}

static void	SwitchHMI( HMI *self, HMI *p_hmi)
{
	HMI		*save_last_case_err = NULL;		//如果切换发生错误，就要恢复之前的旧画面
	if( p_hmi == NULL)
		return;
	if(p_hmi ==  g_p_winHmi ) {
		
		g_p_win_last = self;
	} else if((self != p_hmi) && (self != g_p_winHmi)) {		//切换到不同的界面上，才更新
		save_last_case_err = g_p_lastHmi;
		g_p_lastHmi = g_p_curHmi;
		
	}
	phn_sys.key_weight = 1;
	
	g_p_curHmi = p_hmi;
	
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	
	Set_flag_show(&self->flag, 0);
	self->hide(self);
	self->clean_cmp(self);
	
	re_show:
	p_hmi->initSheet( p_hmi);
	if(p_hmi->flag & HMI_FLAG_ERR)		//切换发生错误，就切回原画面
	{
		g_p_lastHmi = save_last_case_err;
		g_p_curHmi = self;
		p_hmi = self;
		p_hmi->flag |= HMI_FLAG_KEEP;
		
		goto re_show;
		
	}
		
	
	p_hmi->build_component(p_hmi);
	p_hmi->show( p_hmi);
	
	
	Set_flag_show(&p_hmi->flag, 1);
	p_hmi->show_cmp(p_hmi);
	
	p_hmi->flag &= ~HMI_FLAG_KEEP;
	
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
}

static void	SwitchBack( HMI *self)
{
	HMI *nowHmi = g_p_lastHmi;
	if(g_p_lastHmi == NULL)
		return;
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	
	g_p_lastHmi = g_p_curHmi;
	g_p_curHmi = nowHmi;
	
	phn_sys.key_weight = 1;
	Set_flag_show(&self->flag, 0);
	self->hide( self);
	self->clean_cmp(self);
	
	
	nowHmi->initSheet( nowHmi);
	nowHmi->build_component(nowHmi);
	nowHmi->show( nowHmi);
	
	nowHmi->flag &= ~HMI_FLAG_KEEP;
	Set_flag_show(&nowHmi->flag, 1);
	nowHmi->show_cmp(nowHmi);
	
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
}


static void HitHandle( HMI *self, char *s_key)
{
	
}

static void LngpshHandle( HMI *self, char *s_key)
{
	
}

static void DHitHandle( HMI *self, char *s_key)
{
	
}

static void ConposeKeyHandle(HMI *self, char *s_key1, char *s_key2)
{
	if( !strcmp( s_key1, HMIKEY_LEFT) && !strcmp( s_key2, HMIKEY_RIGHT))
	{

		phn_sys.sys_flag |= SYSFLAG_SETTING;
		self->switchHMI(self, g_p_Setup_HMI);
	} 
	else if( !strcmp( s_key1, HMIKEY_RIGHT) && !strcmp( s_key2, HMIKEY_LEFT)) {
		phn_sys.sys_flag |= SYSFLAG_SETTING;
		self->switchHMI(self, g_p_Setup_HMI);
	}
	else if( !strcmp( s_key1, HMIKEY_UP) && !strcmp( s_key2, HMIKEY_DOWN)) {
		self->switchBack(self);
	}
	else if( !strcmp( s_key2, HMIKEY_UP) && !strcmp( s_key1, HMIKEY_DOWN)) {
		self->switchBack(self);
	}
	
	
}

void	Init_focus(HMI *self) 
{
	self->p_fcuu = NULL;
}
void	Clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col) 
{
	sheet *p_sht = Focus_Get_sht(self->p_fcuu, fouse_row, fouse_col);
	
	if(p_sht == NULL)
		return;
	p_sht->cnt.effects = GP_CLR_EFF( p_sht->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_sht);

}
void	Show_focus( HMI *self, uint8_t fouse_row, uint8_t fouse_col) 
{
	sheet *p_sht = Focus_Get_focus(self->p_fcuu);
	
	if(p_sht == NULL)
		return;
	p_sht->cnt.effects = GP_SET_EFF( p_sht->cnt.effects, EFF_FOCUS);
	Sheet_slide( p_sht);
}


static void		HMI_Build_cmp(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	int		i;
	for(i = 0; i < NUM_BUTTON; i++)
	{
		p->build_each_btn(i, BTN_TYPE_NONE, NULL, NULL);


	}

}

static void		HMI_Clean_cmp(HMI *self)
{

	Button				*p = BTN_Get_Sington();
	Progress_bar		*p_bar = PGB_Get_Sington();
	Curve 				*p_crv = CRV_Get_Sington();
	CMP_tips 			*p_tips = TIP_Get_Sington();
	
	p->clean_btn();
	self->flag &= ~HMIFLAG_FOCUS_IN_BTN;
	
	p_bar->delete_bar(HMI_CMP_ALL);
	p_crv->free(HMI_CMP_ALL);
	
	p_tips->hide_ico_tips(0);		//默认提示图标允许显示

}
static void		HMI_Show_cmp(HMI *self)
{
	Button				*p = BTN_Get_Sington();
	Progress_bar		*p_bar = PGB_Get_Sington();
	Curve 				*p_crv = CRV_Get_Sington();
	CMP_tips 			*p_tips = TIP_Get_Sington();
	int					i;
	
	p->show_vaild_btn();
	p_bar->show_bar();
	p_crv->crv_show_bkg();
	p_tips->show_tips();
	
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		if(g_arr_p_chnData[i]->update)
			g_arr_p_chnData[i]->update(g_arr_p_chnData[i], NULL);
		if(g_arr_p_chnUtil[i]->update)
			g_arr_p_chnUtil[i]->update(g_arr_p_chnUtil[i], NULL);
		if(g_arr_p_chnAlarm[i]->update)
			g_arr_p_chnAlarm[i]->update(g_arr_p_chnAlarm[i], NULL);
		
		
	}
}

static int		HMI_Btn_forward(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	int			ret;
	
	ret = p->move_focus(BTN_MOVE_FORWARD);
	if(ret == RET_OK)
		self->flag |= HMIFLAG_FOCUS_IN_BTN;
	else
		self->flag &= ~HMIFLAG_FOCUS_IN_BTN;
	return ret;
}

static int		HMI_Btn_backward(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	
	int			ret;
	
	ret = p->move_focus(BTN_MOVE_BACKWARD);
	
	if(ret == RET_OK)
		self->flag |= HMIFLAG_FOCUS_IN_BTN;
	else
		self->flag &= ~HMIFLAG_FOCUS_IN_BTN;
	
	return ret;
}
static void		HMI_Btn_jumpout(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	p->move_focus(BTN_MOVE_JUMPOUT);
	self->flag &= ~HMIFLAG_FOCUS_IN_BTN;
}

//static void		HMI_Btn_hit(HMI *self)
//{
//	Button	*p = BTN_Get_Sington();
//	p->hit();
//	
//}



