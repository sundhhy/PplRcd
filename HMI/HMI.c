
#include <string.h>

#include "arithmetic/bit.h"
#include "os/os_depend.h"

#include "HMI.h"
#include "HMI_comm.h"
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
//HMI *g_p_lastHmi;
//HMI *g_p_win_last;

keyboard_commit	kbr_cmt = NULL;
//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define KEEP_NUM_HMI		8			//记录的历史HMI数量，当超过的时候，把最后一个替换掉，用于返回操作

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
struct {
//	uint8_t		tip_ico_change;
	uint8_t		tip_ico_status;
	uint8_t		tip_ico_old_status;
	uint8_t		none[2];
}hmi_run;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static HMI *arr_him_histroy[KEEP_NUM_HMI];
static HMI *change_last_hmi;	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void	HmiShow( HMI *self);
static void	HMI_Run( HMI *self);
static void	SwitchHMI( HMI *self, HMI *p_hmi, uint32_t	att_flag);
static void	SwitchBack( HMI *self, uint32_t	att_flag);
static void HitHandle( HMI *self, char kcd);
static void LngpshHandle( HMI *self, char kcd);
static void DHitHandle( HMI *self, char kcd);
static void ConposeKeyHandle(HMI *self, char kcd_1, char kcd_2);

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
static void HMI_Flush(void *);		//定期刷屏


static void HMI_Push_hmi(HMI *h);
static HMI* HMI_Pop_hmi(void);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void Set_flag_show(uint8_t	*p_flag, int val)
{
	
	if(val == 0)
		*p_flag |= HMI_FLAG_HIDE;
	else
		*p_flag &= ~HMI_FLAG_HIDE;
//	val &= 1;
//	*p_flag &= 0xfe;
//	*p_flag |= val;
}



int HMI_Init(void)
{
	
	Button					*p_btn;
	Progress_bar			*p_bar;
	Curve					*p_crv;
	CMP_tips				*p_tips;
	HMI 					*p_hmi;
	HMI 					*p_HMI_main;

	
	
	//初始化界面组件
	p_btn = BTN_Get_Sington();
	p_btn->init(p_btn);
	p_bar = PGB_Get_Sington();
	p_bar->init(p_bar);
	p_crv = CRV_Get_Sington();
	p_crv->init(p_crv, NUM_CHANNEL);
	p_tips = TIP_Get_Sington();
	p_tips->init(p_tips);
	

	phn_sys.hmi_mgr.hmi_sem = Alloc_sem();
	Sem_init(&phn_sys.hmi_mgr.hmi_sem);
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
	
	//初始化各个界面

	
	p_hmi = Create_HMI(HMI_CMM);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_KYBRD);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_MENU);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_BAR);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_DATA);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_RLT_TREND);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_NWS);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_ACCM);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_SETUP);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_STRIPED_BKG);
	p_hmi->init(p_hmi, NULL);
	
	p_hmi = Create_HMI(HMI_WINDOWS);
	p_hmi->init(p_hmi, NULL);
	
	
	p_HMI_main = Create_HMI(HMI_MAIN);
	p_HMI_main->init( p_HMI_main, NULL);
	
	
	p_HMI_main->switchHMI(NULL, p_HMI_main, 0);
	Cmd_Rgt_time_task(HMI_Flush, NULL, HMI_FLUSH_CYCLE_S);
	
	return RET_OK;
}

void HMI_TIP_ICO(uint8_t	type, char ctl)
{
	if(type > 7)
		return;
//	Set_bit(&hmi_run.tip_ico_change, type);
	if(ctl)
		Set_bit(&hmi_run.tip_ico_status, type);
	else
		Clear_bit(&hmi_run.tip_ico_status, type);
	

	
	
}

void	HMI_Attach_model_chn(int  *fds, mdl_observer *mdl_obs)
{
	int			i;
	Model		*p_mdl;
	char		chn_name[8];
	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		
		sprintf(chn_name,"chn_%d", i);
		p_mdl = Create_model(chn_name);
		fds[i] = p_mdl->attach(p_mdl, mdl_obs);
	}
}

void	HMI_detach_model_chn(int  *fds)
{
	
	int			i;
	Model		*p_mdl;
	char		chn_name[8];
	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		
		sprintf(chn_name,"chn_%d", i);
		p_mdl = Create_model(chn_name);
		p_mdl->detach(p_mdl, fds[i]);
	}
}

//void Set_flag_keyhandle(uint8_t	*p_flag, int val)
//{
//	val &= 2;
//	*p_flag &= 0xfd;
//	*p_flag |= val;
//}

void HMI_Change_last_HMI(HMI *p)
{
	
	change_last_hmi = p;
}

void HMI_Updata_tip_ico(void)
{
	

	CMP_tips 			*p_tips = TIP_Get_Sington();
	uint8_t				tips_change = 0;		//

	
	tips_change = hmi_run.tip_ico_old_status ^ hmi_run.tip_ico_status;
	if(Check_bit(&tips_change, TIP_ICO_USB))
	{
		if(Check_bit(&hmi_run.tip_ico_status, TIP_ICO_USB))
			p_tips->show_ico_tips(0, -1);
		else
			p_tips->clear_ico_tips(0);
	}
	
	if(Check_bit(&tips_change, TIP_ICO_CLEAR_FILE))
	{
		if(Check_bit(&hmi_run.tip_ico_status, TIP_ICO_CLEAR_FILE))
			p_tips->show_ico_tips(2, 36);
		else
			p_tips->clear_ico_tips(2);
	}
	
	if(Check_bit(&tips_change, TIP_ICO_WARING))
	{
		if(Check_bit(&hmi_run.tip_ico_status, TIP_ICO_WARING))
			p_tips->show_ico_tips(1, -1);
		else
			p_tips->clear_ico_tips(1);
	}
	
	if(Check_bit(&tips_change, TIP_ICO_ERR))
	{
		if(Check_bit(&hmi_run.tip_ico_status, TIP_ICO_ERR))
			p_tips->show_ico_tips(2, -1);
		else
			p_tips->clear_ico_tips(2);
	}
	
	hmi_run.tip_ico_old_status = hmi_run.tip_ico_status;
	

	
}




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

static void HMI_Flush(void *arg)
{
	Cmd_Rgt_time_task(HMI_Flush, NULL, HMI_FLUSH_CYCLE_S);
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

static void	SwitchHMI( HMI *self, HMI *p_hmi, uint32_t	att_flag)
{
//	HMI		*save_last_case_err = NULL;		//如果切换发生错误，就要恢复之前的旧画面
	if( p_hmi == NULL)
		return;
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;	
	//把本界面切出
		//如果本画面是空值，说明不需要对本画面进行切出操作
		//在上电启动显示第一幅画面时就出现这种情况
	if(self == NULL)
		goto show_target;
		//本画面切出成功，则把本画面装入历史画面列表
			//如果本画面与目标画面相同，那就不必保存到历史列表了	\
				一般是画面需要重新显示的时候会出现这种情况
	Set_flag_show(&self->flag, 0);
	self->hide(self);
	self->clean_cmp(self);
	
	if(self == p_hmi)
		goto show_target; 
	
	if(att_flag & HMI_ATT_NOT_RECORD)
		goto show_target; 
	
	if(change_last_hmi)
	{
		HMI_Push_hmi(change_last_hmi);
		change_last_hmi = NULL;
		
	}
	else
		HMI_Push_hmi(self);
	
	show_target:
	//显示目标画面
		//目标画面显示失败，则要切换回原来的画面
		//把当前画面设置为目标画面
	p_hmi->initSheet( p_hmi, att_flag);
	if(p_hmi->flag & HMI_FLAG_ERR)		//切换发生错误，就切回原画面
	{
		p_hmi = self;
		att_flag = HMI_ATT_KEEP;
		goto show_target;
		
	}
	
	
		
	p_hmi->flag |= HMI_FLAG_HSA_SEM;
	p_hmi->build_component(p_hmi);
	Set_flag_show(&p_hmi->flag, 1);

	
	p_hmi->show( p_hmi);
	p_hmi->show_cmp(p_hmi);
	
	g_p_curHmi = p_hmi;
	p_hmi->flag &= ~HMI_FLAG_HSA_SEM;
	
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
	
	return;
//	err:
//		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
//		self->switchBack(self, HMI_ATT_SELF_ERR | HMI_ATT_KEEP);
	
//	if(p_hmi ==  g_p_winHmi ) {
//		
//		g_p_win_last = self;
//	} else if((self != p_hmi) && (self != g_p_winHmi)) {		//切换到不同的界面上，才更新
//		save_last_case_err = g_p_lastHmi;
//		g_p_lastHmi = g_p_curHmi;
//		
//	}
	

		
	
//	phn_sys.key_weight = 1;
//	
//	
//	
//	
//	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
//		return;
//	
//	if(self == NULL)
//	{
//		goto show_target;
//		
//	}
//	
//	
//	
//	
//	Set_flag_show(&self->flag, 0);
//	self->hide(self);
//	self->clean_cmp(self);
//	
//	if(self != p_hmi)
//	{
//		if(change_last_hmi)
//		{
//			HMI_Push_hmi(change_last_hmi);
//			change_last_hmi = NULL;
//			
//		}
//		else
//			HMI_Push_hmi(self);
//		
//	}
//	
//	
//	
//	show_target:
//	p_hmi->initSheet( p_hmi);
//	if(p_hmi->flag & HMI_FLAG_ERR)		//切换发生错误，就切回原画面
//	{
//		g_p_lastHmi = save_last_case_err;
//		g_p_curHmi = self;
//		p_hmi = self;
//		p_hmi->flag |= HMI_FLAG_KEEP;
//		
//		goto show_target;
//		
//	}
//	
//	
//		
//	p_hmi->flag |= HMI_FLAG_HSA_SEM;
//	p_hmi->build_component(p_hmi);
//	p_hmi->show( p_hmi);
//	
////	p_hmi->show( p_hmi);
//	
//	
//	Set_flag_show(&p_hmi->flag, 1);
//	p_hmi->show_cmp(p_hmi);
////	p_hmi->show_cmp(p_hmi);
//	
//	p_hmi->flag &= ~HMI_FLAG_KEEP;
//	g_p_curHmi = p_hmi;
//	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
//	p_hmi->flag &= ~HMI_FLAG_HSA_SEM;
}

static void	SwitchBack( HMI *self, uint32_t	att_flag)
{
	HMI *p_hmi = HMI_Pop_hmi();
	if(p_hmi == NULL)
		return;
	
	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
		return;
	
	g_p_curHmi = p_hmi;
	
	phn_sys.key_weight = 1;
	Set_flag_show(&self->flag, 0);
	
	if(att_flag & HMI_ATT_SELF_ERR)
		goto show_last;
	self->hide( self);
	self->clean_cmp(self);
	
	show_last:
	p_hmi->flag |= HMI_FLAG_HSA_SEM;
	p_hmi->initSheet(p_hmi, att_flag & ~HMI_ATT_SELF_ERR);
	p_hmi->build_component(p_hmi);
	p_hmi->show(p_hmi);
	
//	p_hmi->flag &= ~HMI_FLAG_KEEP;
	Set_flag_show(&p_hmi->flag, 1);
	p_hmi->show_cmp(p_hmi);
	
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	p_hmi->flag &= ~HMI_FLAG_HSA_SEM;
	
//	HMI *p_hmi = g_p_lastHmi;
//	if(p_hmi == NULL)
//		return;
//	
//	if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0)
//		return;
//	
//	g_p_lastHmi = g_p_curHmi;
//	g_p_curHmi = p_hmi;
//	
//	phn_sys.key_weight = 1;
//	Set_flag_show(&self->flag, 0);
//	
//	
//	self->hide( self);
//	self->clean_cmp(self);
//	
//	p_hmi->flag |= HMI_FLAG_HSA_SEM;
//	p_hmi->initSheet( p_hmi);
//	p_hmi->build_component(p_hmi);
//	p_hmi->show( p_hmi);
//	
//	p_hmi->flag &= ~HMI_FLAG_KEEP;
//	Set_flag_show(&p_hmi->flag, 1);
//	p_hmi->show_cmp(p_hmi);
//	
//	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
//	p_hmi->flag &= ~HMI_FLAG_HSA_SEM;
}


static void HitHandle( HMI *self, char kcd)
{
	
}

static void LngpshHandle( HMI *self, char kcd)
{
	
}

static void DHitHandle( HMI *self, char kcd)
{
	
}

static void ConposeKeyHandle(HMI *self, char kcd_1, char kcd_2)
{
	HMI *p_hsb = Create_HMI(HMI_SETUP);
		if(kcd_1 == KEYCODE_RIGHT && kcd_2 == KEYCODE_LEFT)
		{
//			phn_sys.sys_flag |= SYSFLAG_SETTING;
			self->switchHMI(self, p_hsb, 0);

		} 
		else if(kcd_2 == KEYCODE_RIGHT && kcd_1 == KEYCODE_LEFT) {
//			phn_sys.sys_flag |= SYSFLAG_SETTING;
			self->switchHMI(self, p_hsb, 0);
		}
		else if(kcd_2 == KEYCODE_UP && kcd_1 == KEYCODE_ENTER) {
			self->switchBack(self, 0);

		}
		else if(kcd_2 == KEYCODE_ENTER && kcd_1 == KEYCODE_UP) {
			self->switchBack(self, 0);

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
	
	p->show_vaild_btn();
	p_bar->show_bar();
	p_crv->crv_show_bkg();
	p_tips->show_tips();
	

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
static void HMI_Push_hmi(HMI *h)
{
	int i = 0;
	while(i < KEEP_NUM_HMI)
	{
		
		if(arr_him_histroy[i] == NULL)
			break;
		i ++;
	}
	
	if(i == KEEP_NUM_HMI)
		i = KEEP_NUM_HMI - 1;
	
	arr_him_histroy[i] = h;
}
static HMI* HMI_Pop_hmi(void)
{
	HMI	*r = NULL;
	int i = 0;
	while(i < KEEP_NUM_HMI)
	{
		
		if(arr_him_histroy[i] == NULL)
			break;
		i ++;
	}
	
	
	
	if(i > 0)
	{
		i --;
		r = arr_him_histroy[i];
		
		arr_him_histroy[i] = NULL;
	}
	
	return r;
	
}


