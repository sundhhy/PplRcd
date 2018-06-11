#include "HMI_barGraph.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "ModelFactory.h"
#include "arithmetic/bit.h"


//柱状图在y坐标上，按100%显示的话是:71 -187 
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_barGhHmi;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define BARHMI_NUM_BTNROW		1
#define BARHMI_NUM_BTNCOL		4
#define VRAM_RUN_NUM				0

#define BARHMI_BKPICNUM		"12"
#define BARHMI_TITLE		"棒图画面"

static const char barhmi_code_clean[] = { "<cpic vx0=0 vy0=0 >12</>" };
static const char barhmi_code_bar[] = { "<box bx=35 ></>" };
static const char barhmi_code_textPrcn[] = { "<text f=16 m=0>100</>" };


//static const hmiAtt_t	barHmiAtt = { 4,4, COLOUR_GRAY, BARHMI_NUM_BTNROW, BARHMI_NUM_BTNCOL};


//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
typedef struct {
	int			mdf_fd[NUM_CHANNEL];
	
	//让通道的数据更新与界面更新实现异步处理
	uint8_t		set_need_update_channel[4];			//每个bit对应一个通道，为1表示需要更新通道
	void *		arr_p_need_update_model[NUM_CHANNEL];
}bar_run_t;

	
//static sheet  *arr_p_sht_select[BARHMI_NUM_BTNCOL];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_barGhHMI( HMI *self, void *arg);
static void BarHmi_InitSheet( HMI *self, uint32_t att );
static void BarHmi_HideSheet( HMI *self );
static void BAR_Run(HMI* self);

static void	BarHmi_Show( HMI *self);
static int HBR_Update_mdl_chn_data(mdl_observer *self, void *p_srcMdl);

//static void	BarHmi_HitHandle( HMI *self, char *s);


//柱形图操作函数
static void BarHmi_Init_chnSht(void);
static void Init_bar( HMI_bar *self);
static int BarHmi_Data_update(void *p_data, Model *p_mdl);
static int BarHmi_Util_update(void *p_data, Model *p_mdl);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

HMI_bar *Get_barGhHMI(void)
{
	static HMI_bar *singal_barHmi = NULL;
	if( singal_barHmi == NULL)
	{
		singal_barHmi = HMI_bar_new();
		if(singal_barHmi  == NULL) while(1);
		g_p_barGhHmi = SUPER_PTR( singal_barHmi, HMI);
		
	}
	
	return singal_barHmi;
	
}

CTOR( HMI_bar)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_barGhHMI);
FUNCTION_SETTING( HMI.initSheet, BarHmi_InitSheet);
FUNCTION_SETTING( HMI.hide, BarHmi_HideSheet);
FUNCTION_SETTING( HMI.show, BarHmi_Show);
FUNCTION_SETTING( HMI.hmi_run, BAR_Run);

FUNCTION_SETTING( HMI.hitHandle, Main_HMI_hit);
FUNCTION_SETTING(HMI.build_component, Main_HMI_build_button);
FUNCTION_SETTING( mdl_observer.update, HBR_Update_mdl_chn_data);

END_CTOR


//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_barGhHMI( HMI *self, void *arg)
{
	HMI_bar		*cthis = SUB_PTR( self, HMI, HMI_bar);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
	
//	p_shtctl = GetShtctl();
	
	//初始化背景图片
//	p_exp = ExpCreate( "pic");
//	cthis->p_bar_clean = Sheet_alloc( p_shtctl);
//	p_exp->inptSht( p_exp, (void *)barhmi_code_clean, cthis->p_bar_clean) ;
	
	
	
	//初始化柱形图
	Init_bar(cthis);
	
	self->flag = 0;

	
	
	
	return RET_OK;

}

static void BAR_Run(HMI* self)
{
	bar_run_t *p_run = (bar_run_t *)arr_p_vram[VRAM_RUN_NUM];
	int 	chn_num;
	
	
	if((self->flag & HMI_FLAG_HSA_SEM) == 0)
	{
		
		if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 100) <= 0) 
		{
			return;	//下次在更新
		}
	
	}
	
	for(chn_num = 0; chn_num < phn_sys.sys_conf.num_chn; chn_num ++)
	{
		
		if(Check_bit(p_run->set_need_update_channel, chn_num) == 0)
			continue;
		
		
		
		//更新实时值
		BarHmi_Data_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		
		//更新单位
		BarHmi_Util_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		//更新报警
		
		Clear_bit(p_run->set_need_update_channel, chn_num);
		
	}
	
	HMI_Updata_tip_ico();
	if((self->flag & HMI_FLAG_HSA_SEM) == 0)
		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
}

static void BarHmi_InitSheet( HMI *self, uint32_t att )
{
	HMI_bar			*cthis = SUB_PTR( self, HMI, HMI_bar);
	int i,  h = 0;
	Expr 			*p_exp ;
	bar_run_t 		*p_run;
	
	p_exp = ExpCreate( "pic");
	p_exp->inptSht( p_exp, (void *)barhmi_code_clean, g_p_cpic) ;
	

	


	g_p_sht_bkpic->cnt.data = BARHMI_BKPICNUM;

	g_p_sht_title->cnt.data = BARHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(BARHMI_TITLE);
	
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
	for(i = 0; i < BARHMI_NUM_BARS; i++) {
//		Sheet_updown(cthis->arr_p_barshts[i], h++);
//		Sheet_updown(g_arr_p_chnData[i], h++);
//		Sheet_updown(g_arr_p_chnUtil[i], h++);

		
		cthis->arr_bar_height[i] = 0xffff;
	}
	
//	Sheet_updown(g_p_ico_memu, h++);
//	Sheet_updown(g_p_ico_bar, h++);
//	Sheet_updown(g_p_ico_digital, h++);
//	Sheet_updown(g_p_ico_trend, h++);
	
	
	HMI_Ram_init();
		
	arr_p_vram[VRAM_RUN_NUM] = HMI_Ram_alloc(48);
	p_run = (bar_run_t *)arr_p_vram[VRAM_RUN_NUM];
	HMI_Attach_model_chn(p_run->mdf_fd, &cthis->mdl_observer);
	BarHmi_Init_chnSht();
	//初始化焦点
	self->init_focus(self);
}

static void BarHmi_HideSheet( HMI *self )
{
//	HMI_bar			*cthis = SUB_PTR( self, HMI, HMI_bar);
	bar_run_t 		*p_run;
	

	
	
	p_run = (bar_run_t *)arr_p_vram[VRAM_RUN_NUM];
	HMI_detach_model_chn(p_run->mdf_fd);
	

	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	
	self->clear_focus( self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
	
}	





static void	BarHmi_Show( HMI *self )
{
	HMI_bar		*cthis = SUB_PTR( self, HMI, HMI_bar);
	int 			i;
	char		chn_name[8];
	Model		*p_mdl;
	
	
//	g_p_curHmi = self;
	
	//刷新了背景，就要重新开始绘制
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		
		cthis->arr_bar_height[i] = 0xffff;
	}
	
//	Cal_bar( cthis);
	
	Sheet_refresh(g_p_sht_bkpic);
//	self->show_focus( self, 0, 0);
	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		
		sprintf(chn_name,"chn_%d", i);
		p_mdl = Create_model(chn_name);
		cthis->mdl_observer.update(&cthis->mdl_observer, p_mdl);
	}
	
	self->hmi_run(self);
}


static void Init_bar( HMI_bar *self)
{
	
	Expr 			*p_exp ;
	shtctl 			*p_shtctl = NULL;
	char 			i;
//	char			chn_name[7];
	
	p_shtctl = GetShtctl();

	for( i = 0; i < BARHMI_NUM_BARS; i++) {
		p_exp = ExpCreate("box");
		self->arr_p_barshts[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)barhmi_code_bar, self->arr_p_barshts[i]) ;
				
		self->arr_p_barshts[i]->cnt.bkc = arr_clrs[i];
		self->arr_p_barshts[i]->cnt.colour = arr_clrs[i];
		
	}
	
}



static void BarHmi_Init_chnSht(void)
{
	Expr 		*p_exp ;
	int			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		p_exp->inptSht( p_exp, (void *)barhmi_code_textPrcn, g_arr_p_chnData[i]) ;
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnData[i]->cnt.data = prn_buf[i];
	
	}
}


static int HBR_Update_mdl_chn_data(mdl_observer *self, void *p_srcMdl)
{
//	HMI_bar		*cthis = SUB_PTR( self, mdl_observer, HMI_bar);
//	HMI		*p_hmi = SUPER_PTR(cthis, HMI);
	Model		*p_mdl = (Model *)p_srcMdl;
	bar_run_t *p_run = (bar_run_t *)arr_p_vram[VRAM_RUN_NUM];
	short	chn_num = GET_MDL_CHN(p_mdl->mdl_id);
	
	
	Set_bit(p_run->set_need_update_channel, chn_num);
	p_run->arr_p_need_update_model[chn_num] = p_srcMdl;
	
//	if((p_hmi->flag & HMI_FLAG_HSA_SEM) == 0)
//	{
//		if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 100) <= 0) 
//			return ERR_RSU_BUSY;
//	
//	}
//	//更新实时值
//	BarHmi_Data_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	
//	//更新单位
//	BarHmi_Util_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	
//	
//	if((p_hmi->flag & HMI_FLAG_HSA_SEM) == 0)
//		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
	
	return RET_OK;
}



static int BarHmi_Data_update(void *p_data, Model *p_mdl)
{
	
	HMI_bar *chis = Get_barGhHMI();
	sheet	*p_sht = (sheet *)p_data;

	uint16_t bar_vx0[BARHMI_NUM_BARS] = { 30, 78, 126, 172, 220, 268};
	
	uint16_t bar_vy1= 187;
	uint16_t max_height= 116;
	uint16_t text_vy0 = 32;
	uint16_t i = p_sht->id;

	
	uint16_t	height = 0;
	uint8_t		prcn = 0;

	



//	if(IS_HMI_KEYHANDLE(g_p_barGhHmi->flag))
//		return 0;	
//	
//	if(Sheet_is_hide(p_sht))
//		return 0;
//	if(IS_HMI_HIDE(g_p_barGhHmi->flag))
//		return 0;
//	
	
	p_mdl->getMdlData(p_mdl, AUX_PERCENTAGE,  &prcn);
	height = max_height * prcn / 100;
	
	//	p_sht->p_mdl->to_string(p_sht->p_mdl, p_sht->cnt.mdl_aux, p_sht->cnt.data);
	sprintf(p_sht->cnt.data, "%%%3d", prcn);
	p_sht->cnt.len = strlen(p_sht->cnt.data);
	p_sht->area.x0 = bar_vx0[i];
	p_sht->area.y0 = text_vy0;
	Sheet_force_slide(p_sht);
	
	//180213 优化棒图的显示，根据当前高度与上一次高度来计算本次需要更改的部分
	if(chis->arr_bar_height[i] == height)
		return 0;
	
	if(chis->arr_bar_height[i] > max_height)
	{
		//第一次显示
		chis->arr_p_barshts[i]->area.x0 = bar_vx0[i];
		chis->arr_p_barshts[i]->area.x1 = bar_vx0[i] + chis->arr_p_barshts[i]->bxsize;
		
		chis->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
		chis->arr_p_barshts[i]->area.y1 = bar_vy1;
		
		
		Sheet_force_slide( chis->arr_p_barshts[i]);
	}
	else if(chis->arr_bar_height[i] > height)
	{
		//棒图变短
		g_p_cpic->area.x0 = bar_vx0[i];
		g_p_cpic->area.x1 = bar_vx0[i] + chis->arr_p_barshts[i]->bxsize + 5;
		
		
//		g_p_cpic->area.y0 = bar_vy1 - max_height;
//		g_p_cpic->area.y1 = bar_vy1;
		g_p_cpic->area.y0 = bar_vy1 - chis->arr_bar_height[i];
		g_p_cpic->area.y1 = bar_vy1 - height;

		
		g_p_cpic->area.offset_x = 0;
		g_p_cpic->area.offset_y = 0;
		g_p_cpic->p_gp->vdraw(g_p_cpic->p_gp, &g_p_cpic->cnt, &g_p_cpic->area);
		
	}
	else
	{
		//棒图变长
		chis->arr_p_barshts[i]->area.x0 = bar_vx0[i];
		chis->arr_p_barshts[i]->area.x1 = bar_vx0[i] + chis->arr_p_barshts[i]->bxsize;
		
//		chis->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
//		chis->arr_p_barshts[i]->area.y1 = bar_vy1;
		chis->arr_p_barshts[i]->area.y0 = bar_vy1 - height;
		chis->arr_p_barshts[i]->area.y1 = bar_vy1 - chis->arr_bar_height[i];
		
		Sheet_force_slide( chis->arr_p_barshts[i]);
	}
	chis->arr_bar_height[i] = height;
	return 0;	
}

static int BarHmi_Util_update(void *p_data, Model *p_mdl)
{
	sheet		*p_sht = (sheet *)p_data;
	HMI_bar *self = Get_barGhHMI();
	uint16_t bar_vx0[BARHMI_NUM_BARS] = { 30, 78, 126, 172, 220, 268};
	
	uint16_t utit_vy0 = 48;
	uint16_t i = p_sht->id;
	
	
//	if(IS_HMI_HIDE(g_p_barGhHmi->flag))
//		return 0;
//	if(IS_HMI_KEYHANDLE(g_p_barGhHmi->flag))
//		return 0;
		
	p_sht->cnt.data = p_mdl->to_string(p_mdl, AUX_UNIT, NULL);
	p_sht->cnt.len = strlen(p_sht->cnt.data);
	p_sht->area.x0 = bar_vx0[i];
	p_sht->area.y0 = utit_vy0;
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	
	
	Sheet_force_slide( p_sht);
	return 0;
	
}

