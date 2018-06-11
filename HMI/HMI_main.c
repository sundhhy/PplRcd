#include "HMI_main.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "focus.h"
#include "Component_Button.h"
#include "Component_curve.h"

#include "Component_tips.h"
#include "sys_cmd.h"
#include "arithmetic/bit.h"

//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define CHN_ROW			2
#define CHN_COL			3
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_HMI_main;

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define MAINHMI_BKPICNUM		"11"
#define	MAINHMI_TITLE		"总貌画面"
#define VRAM_RUN_NUM			0


//每个通道的单位
static ro_char MAIN_hmi_code_data[] = { "<text f=24 m=0>100</>" };
//static ro_char MAIN_hmi_code_unit[] = { "<text f=16 m=0>m3/h</>" };
//通道报警:HH HI LI LL
//static ro_char MAIN_hmi_code_alarm[] = { "<text f=16 m=0> </>" };

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	int			mdf_fd[NUM_CHANNEL];			//通道模型的观察者句柄，用于界面退出时将本界面的管擦着删除
	
	//让通道的数据更新与界面更新实现异步处理
	uint8_t		set_need_update_channel[4];			//每个bit对应一个通道，为1表示需要更新通道
	void *		arr_p_need_update_model[NUM_CHANNEL];
	
	//记录上一次更新的通道，然后每次更新都从上一次通道之后进行更新
	//这样俩避免出现后面的通道被“饿死”的情况
	uint8_t		last_updat_chn;	
	uint8_t		none[3];
//	int			test;		
}main_run_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static HMI_main *signal_HMI_main;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_HMI_main( HMI *self, void *arg);
static void	MainHmiShow( HMI *self);
static void MainHmiHide( HMI *self );
static void MaininitSheet( HMI *self, uint32_t att );
static void HMI_MAIN_Run(HMI *self);
static int HMN_Update_mdl_chn_data(mdl_observer *self, void *p_srcMdl);
//static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total);


static void MainHmi_Init_chnShet(void);
static int MainHmi_Data_update(void *p_data, Model *p_mdl);
static int MainHmi_Util_update(void *p_data, Model *p_mdl);
static int MainHmi_Alarm_update(void *p_data, Model *p_mdl);

static void	HMN_Init_vram(void);

	

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
HMI_main *Get_HMI_main(void) 
{
	if( signal_HMI_main == NULL)
	{
		signal_HMI_main = HMI_main_new();
		if(signal_HMI_main  == NULL) while(1);
		g_p_HMI_main = SUPER_PTR( signal_HMI_main, HMI);
	}
	
	return signal_HMI_main;
}

void Main_HMI_build_button(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	
	p->build_each_btn(0, BTN_TYPE_MENU, Main_btn_hdl, self);
	p->build_each_btn(1, BTN_TYPE_BAR, Main_btn_hdl, self);
	p->build_each_btn(2, BTN_TYPE_DIGITAL, Main_btn_hdl, self);
	p->build_each_btn(3, BTN_TYPE_TREND, Main_btn_hdl, self);
	
	p->move_focus(0);
}

void	Main_HMI_hit( HMI *self, char kcd)
{

	Button	*p = BTN_Get_Sington();
	
	
	
	
	switch(kcd)
	{

			case KEYCODE_UP:
					 
					break;
			case KEYCODE_DOWN:
					
					break;
			case KEYCODE_LEFT:
				
					//按钮移出屏幕的时候，再次移动一次，避免按钮出现不被选中的情况
					if(self->btn_backward(self) != RET_OK)
						self->btn_backward(self);
					break;
			case KEYCODE_RIGHT:
					if(self->btn_forward(self) != RET_OK)
						self->btn_forward(self);
					break;

			case KEYCODE_ENTER:
					p->hit();
					break;		
			case KEYCODE_ESC:
					self->switchBack(self, 0);
					break;	
			
	}

	
}

void Main_btn_hdl(void *arg, uint8_t btn_id)
{
	HMI					*self	= (HMI *)arg;		
	
	switch(btn_id)
	{
		
		case ICO_ID_MENU:
			self->switchHMI(self, g_p_HMI_menu, 0);
			break;
		case ICO_ID_BAR:
			self->switchHMI(self, g_p_barGhHmi, 0);
			break;	
		case ICO_ID_DIGITAL:
			self->switchHMI(self, g_p_dataHmi, 0);
			break;
		case ICO_ID_TREND:
			g_p_RLT_trendHmi->arg[0] = 0;
			self->switchHMI(self, g_p_RLT_trendHmi, 0);
			break;	
			
	}

	
		
}




CTOR( HMI_main)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_HMI_main);
FUNCTION_SETTING( HMI.hide, MainHmiHide);
FUNCTION_SETTING( HMI.initSheet, MaininitSheet);
FUNCTION_SETTING( HMI.hmi_run, HMI_MAIN_Run);

FUNCTION_SETTING( HMI.show, MainHmiShow);
FUNCTION_SETTING( HMI.hitHandle, Main_HMI_hit);


FUNCTION_SETTING(HMI.build_component, Main_HMI_build_button);
FUNCTION_SETTING(mdl_observer.update, HMN_Update_mdl_chn_data);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_HMI_main( HMI *self, void *arg)
{
//	HMI 					*p_cmm;
//	Button					*p_btn;
//	Progress_bar			*p_bar;
//	Curve					*p_crv;
//	CMP_tips				*p_tips;

//	
//	p_cmm = Create_HMI( HMI_CMM);
//	p_cmm->init( p_cmm, NULL);
//	self->flag = 0;
//	//初始化界面组件
//	p_btn = BTN_Get_Sington();
//	p_btn->init(p_btn);
//	p_bar = PGB_Get_Sington();
//	p_bar->init(p_bar);
//	p_crv = CRV_Get_Sington();
//	p_crv->init(p_crv, NUM_CHANNEL);
//	p_tips = TIP_Get_Sington();
//	p_tips->init(p_tips);
//	
//	self->initSheet( self);

//	phn_sys.hmi_mgr.hmi_sem = Alloc_sem();
//	Sem_init(&phn_sys.hmi_mgr.hmi_sem);
//	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	return RET_OK;
	

}


static void HMI_MAIN_Run(HMI *self)
{
	main_run_t *p_run = (main_run_t *)arr_p_vram[VRAM_RUN_NUM];
	int 	chn_num;
	
	
//	if((self->flag & HMI_FLAG_HSA_SEM) == 0)
//	{
//		//这种情况是界面切换时需要理解刷新一下屏幕时出现的
//		//因此这时候就要把全部的通道都刷新一次
//		p_run->last_updat_chn = 0;
//		
//	
//	}
	
	if((self->flag & HMI_FLAG_HSA_SEM) == 0)
	{
		
		if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 50) <= 0) 
		{
			return;	//下次在更新
		}
	
	}
	
	
	for(chn_num = 0; chn_num < phn_sys.sys_conf.num_chn; chn_num ++)
	{
		
		if(Check_bit(p_run->set_need_update_channel, chn_num) == 0)
			continue;
		
		
		
		//更新实时值
		MainHmi_Data_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		
		//更新单位
		MainHmi_Util_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		//更新报警
		MainHmi_Alarm_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		
		
		Clear_bit(p_run->set_need_update_channel, chn_num);
//		p_run->last_updat_chn = chn_num + 1;
//		p_run->last_updat_chn %= phn_sys.sys_conf.num_chn;
		
//		else
//			break;		//正常情况一次更新一个通道，因为存储更重要

	}
	
	
	HMI_Updata_tip_ico();
	if((self->flag & HMI_FLAG_HSA_SEM) == 0)
		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
	
	
	
	
	
}



static void MainHmiHide( HMI *self )
{
//	HMI_main		*p = SUB_PTR( self, HMI, HMI_main);
	main_run_t 		*p_run;
	

	p_run = (main_run_t *)arr_p_vram[VRAM_RUN_NUM];
	HMI_detach_model_chn(p_run->mdf_fd);
	
	
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	self->clear_focus(self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
//	Focus_free(self->p_fcuu);
	
}	

static void MaininitSheet( HMI *self, uint32_t att )
{
	HMI_main		*p = SUB_PTR( self, HMI, HMI_main);
	main_run_t 		*p_run;
	short h = 0;
	
	h = 0;
	g_p_sht_bkpic->cnt.data = MAINHMI_BKPICNUM;

	g_p_sht_title->cnt.data = MAINHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(MAINHMI_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);


	
	HMN_Init_vram();
	p_run = (main_run_t *)arr_p_vram[VRAM_RUN_NUM];
	memset(p_run->set_need_update_channel, 0, sizeof(p_run->set_need_update_channel));
	p_run->last_updat_chn = 0;
	HMI_Attach_model_chn(p_run->mdf_fd, &p->mdl_observer);

	
	MainHmi_Init_chnShet();
	self->init_focus(self);
	
}


static void	MainHmiShow( HMI *self )
{
	HMI_main		*cthis = SUB_PTR( self, HMI, HMI_main);
	int		i = 0;
	char		chn_name[8];
	Model		*p_mdl;
	
	
	g_p_curHmi = self;

	
	Sheet_refresh(g_p_sht_bkpic);

	
	//刚切入画面时，模型可能没有更新
	//这里主动显示一次,否则可能会出现空洞
	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		
		sprintf(chn_name,"chn_%d", i);
		p_mdl = Create_model(chn_name);
		cthis->mdl_observer.update(&cthis->mdl_observer, p_mdl);
	}
	
	self->hmi_run(self);
}






static void MainHmi_Init_chnShet(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	int			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_data, g_arr_p_chnData[i]) ;
//		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_unit, g_arr_p_chnUtil[i]) ;
//		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_alarm, g_arr_p_chnAlarm[i]) ;
//		
		g_arr_p_chnData[i]->id = i;
//		g_arr_p_chnUtil[i]->id = i;
//		g_arr_p_chnAlarm[i]->id = i;
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
//		g_arr_p_chnUtil[i]->cnt.colour = arr_clrs[i];
//		g_arr_p_chnAlarm[i]->cnt.colour = arr_clrs[i];
		
		
		
//		g_arr_p_chnData[i]->update = MainHmi_Data_update;
//		g_arr_p_chnUtil[i]->update = MainHmi_Util_update;
//		g_arr_p_chnAlarm[i]->update = MainHmi_Alarm_update;


	
	}
}



static int HMN_Update_mdl_chn_data(mdl_observer *self, void *p_srcMdl)
{
//	HMI_main		*cthis = SUB_PTR( self, mdl_observer, HMI_main);
//	HMI		*p_hmi = SUPER_PTR(cthis, HMI);
	Model	*p_mdl = (Model *)p_srcMdl;
	main_run_t *p_run = (main_run_t *)arr_p_vram[VRAM_RUN_NUM];
	short	chn_num = GET_MDL_CHN(p_mdl->mdl_id);
	
	
	Set_bit(p_run->set_need_update_channel, chn_num);
	p_run->arr_p_need_update_model[chn_num] = p_srcMdl;
//	
//	if((p_hmi->flag & HMI_FLAG_HSA_SEM) == 0)
//	{
//		
//		if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 100) <= 0) 
//		{
//			return ERR_RSU_BUSY;
//		}
//	
//	}
//	//更新实时值
//	MainHmi_Data_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	
//	//更新单位
//	MainHmi_Util_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	//更新报警
//	MainHmi_Alarm_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	
//	if((p_hmi->flag & HMI_FLAG_HSA_SEM) == 0)
//		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
	
	return RET_OK;
}
static int MainHmi_Data_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 32;
	uint8_t		right_x = 106;
	uint8_t		box_sizey = 84;		
	uint8_t		box_sizex = 106;		
	
	//到四周边界的空隙
	uint8_t		space_to_up = 		box_sizey/3;	
//	uint8_t		space_to_bottom = 	0;
//	uint8_t		space_to_left = 	0;	
	uint8_t		space_to_right = 	8;
	
	char 			i = 0, j = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
	
	i = p_sht->id % 3;
	j = p_sht->id / 3;
	
//	if(IS_HMI_HIDE(g_p_HMI_main->flag))
//		return 0;

	p_sht->cnt.data = \
		p_mdl->to_string(p_mdl, AUX_DATA, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

	p_sht->area.x0 = right_x +  (i ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 = up_y + j * box_sizey + space_to_up;
	
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	Sheet_force_slide(p_sht);
	return 0;
	
}

static int MainHmi_Util_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 32;
	uint8_t		right_x = 106;
	uint8_t		box_sizey = 84;		
	uint8_t		box_sizex = 106;		
	
	//到四周边界的空隙
//	uint8_t		space_to_up = 		0;	
	uint8_t		space_to_bottom = 	2;
//	uint8_t		space_to_left = 	0;	
	uint8_t		space_to_right = 	8;
	
	char 			i = 0, j = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
//	if(IS_HMI_HIDE(g_p_HMI_main->flag))
//		return 0;
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_mdl->to_string(p_mdl, AUX_UNIT, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

	p_sht->area.x0 = right_x +  (i ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 = up_y  + (j + 1) * box_sizey  -( sizey + space_to_bottom);
	
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	Sheet_force_slide( p_sht);
	return 0;
	
}

static int MainHmi_Alarm_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 32;
	uint8_t		right_x = 106;
	uint8_t		box_sizey = 84;		
	uint8_t		box_sizex = 106;		
	
	//到四周边界的空隙
	uint8_t		space_to_up = 		2;	
//	uint8_t		space_to_bottom = 	0;
//	uint8_t		space_to_left = 	0;	
	uint8_t		space_to_right = 	0;
	
	char 			i = 0, j = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
//	if(IS_HMI_HIDE(g_p_HMI_main->flag))
//		return 0;
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_mdl->to_string(p_mdl, AUX_ALARM, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

//	p_sht->area.x0 = (i ) * box_sizex + space_to_left;
	p_sht->area.x0 = right_x +  (i ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 =  up_y  + (j ) * box_sizey  + ( 0 + space_to_up);
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	Sheet_force_slide( p_sht);
	return 0;
	
}

static void	HMN_Init_vram(void)
{
	HMI_Ram_init();
		
	arr_p_vram[VRAM_RUN_NUM] = HMI_Ram_alloc(sizeof(main_run_t));
	
}





