#include "HMI_data.h"
#include "HMI_menu.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "ModelFactory.h"
#include "arithmetic/bit.h"


//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

HMI *g_p_dataHmi;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------





//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define		DATAHMI_BKPICNUM		"13"

#define		DATAHMI_TITLE	"数显画面"

//static const char datahmi_code_bkPic[] =  {"<bpic vx0=0 vy0=0 m=0 >21</>" };
static const char datahmi_code_data[] = { "<text f=32 m=0>100</>" };



//static const hmiAtt_t	barHmiAtt = { 4,4, COLOUR_GRAY, BARHMI_NUM_BTNROW, BARHMI_NUM_BTNCOL};

#define VRAM_RUN_NUM				0
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	int			mdf_fd[NUM_CHANNEL];
	//让通道的数据更新与界面更新实现异步处理
	uint8_t		set_need_update_channel[4];			//每个bit对应一个通道，为1表示需要更新通道
	void *		arr_p_need_update_model[NUM_CHANNEL];
}data_run_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_dataHMI( HMI *self, void *arg);
static void DataHmi_InitSheet( HMI *self, uint32_t att );
static void DataHmi_HideSheet( HMI *self );
static void DATA_Run(HMI *self);
static void	DataHmi_Show( HMI *self);
static int HDT_Update_mdl_chn_data(mdl_observer *self, void *p_srcMdl);

static void	DataHmi_HitHandle( HMI *self, char kcd);

//焦点






//static int DataHmi_MdlUpdata( mdl_observer *self, void *p_srcMdl);
//static void Bulid_dataSheet( HMI_data *self);
//static void DataHmi_update(HMI_data *self);
//通道数据
static void DataHmi_Init_chnSht(void);
static int DataHmi_Data_update(void *p_data, Model *p_mdl);
static int DataHmi_Util_update(void *p_data, Model *p_mdl);
static int DataHmi_Alarm_update(void *p_data, Model *p_mdl);
static int DataHmi_Status_update(void *p_data, Model *p_mdl);



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

HMI_data *Get_dataHMI(void)
{
	static HMI_data *singal_dataHmi = NULL;
	if( singal_dataHmi == NULL)
	{
		singal_dataHmi = HMI_data_new();
		if(singal_dataHmi  == NULL) while(1);
		g_p_dataHmi = SUPER_PTR( singal_dataHmi, HMI);
		
	}
	
	return singal_dataHmi;
	
}

CTOR( HMI_data)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_dataHMI);
FUNCTION_SETTING( HMI.initSheet, DataHmi_InitSheet);
FUNCTION_SETTING( HMI.hide, DataHmi_HideSheet);
FUNCTION_SETTING( HMI.show, DataHmi_Show);
FUNCTION_SETTING( HMI.hmi_run, DATA_Run);

FUNCTION_SETTING( HMI.hitHandle, DataHmi_HitHandle);

FUNCTION_SETTING( mdl_observer.update, HDT_Update_mdl_chn_data);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_dataHMI( HMI *self, void *arg)
{
//	HMI_data		*cthis = SUB_PTR( self, HMI, HMI_data);
//	Expr 			*p_exp ;
//	shtctl 			*p_shtctl = NULL;
	
//	p_shtctl = GetShtctl();
	
	//初始化背景图片
//	p_exp = ExpCreate( "pic");
	
	self->flag = 0;
//	Bulid_dataSheet(cthis);

	//初始化焦点
//	self->init_focus(self);
//	cthis->flags = 0;
	
	return RET_OK;

}

static void DATA_Run(HMI *self)
{
	data_run_t *p_run = (data_run_t *)arr_p_vram[VRAM_RUN_NUM];
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
		DataHmi_Data_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		
		//更新单位
		DataHmi_Util_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		//更新报警
		DataHmi_Alarm_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num] );
		
		//更新通道状态
		if(phn_sys.sys_conf.show_chn_status)
			DataHmi_Status_update(g_arr_p_chnData[chn_num], p_run->arr_p_need_update_model[chn_num]);
		Clear_bit(p_run->set_need_update_channel, chn_num);
		
	}
	HMI_Updata_tip_ico();
	if((self->flag & HMI_FLAG_HSA_SEM) == 0)
		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
}



static void DataHmi_InitSheet( HMI *self, uint32_t att )
{
	HMI_data			*cthis = SUB_PTR( self, HMI, HMI_data);
	int  h = 0;
	data_run_t 		*p_run;
	
	g_p_sht_bkpic->cnt.data = DATAHMI_BKPICNUM;

	g_p_sht_title->cnt.data = DATAHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(DATAHMI_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
//	for( i = 0; i < BARHMI_NUM_BARS; i++) {
//		Sheet_updown(g_arr_p_chnData[i], h++);
//		Sheet_updown(g_arr_p_chnUtil[i], h++);
//		Sheet_updown(g_arr_p_chnAlarm[i], h++);
//	}
	
	HMI_Ram_init();
		
	arr_p_vram[VRAM_RUN_NUM] = HMI_Ram_alloc(sizeof(data_run_t));
	p_run = (data_run_t *)arr_p_vram[VRAM_RUN_NUM];
	memset(p_run->set_need_update_channel, 0, sizeof(p_run->set_need_update_channel));
	DataHmi_Init_chnSht();
	HMI_Attach_model_chn(p_run->mdf_fd, &cthis->mdl_observer);
	
	
}

static void DataHmi_HideSheet( HMI *self )
{
//	HMI_data			*cthis = SUB_PTR( self, HMI, HMI_data);
	
//	int i;
	data_run_t 		*p_run;

//	cthis->flags = 0;
//	for( i = BARHMI_NUM_BARS - 1; i >= 0; i--) {
//		Sheet_updown(g_arr_p_chnData[i], -1);
//		Sheet_updown(g_arr_p_chnUtil[i], -1);
//		Sheet_updown(g_arr_p_chnAlarm[i], -1);
//	}
	
	p_run = (data_run_t *)arr_p_vram[VRAM_RUN_NUM];
	HMI_detach_model_chn(p_run->mdf_fd);
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
//	self->clear_focus(self, 0, 0);
}	





static void	DataHmi_Show( HMI *self )
{
	HMI_data		*cthis = SUB_PTR( self, HMI, HMI_data);
	int		i = 0;
	char		chn_name[8];
	Model		*p_mdl;
	
	g_p_curHmi = self;

	Sheet_refresh(g_p_sht_bkpic);

	
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		
		sprintf(chn_name,"chn_%d", i);
		p_mdl = Create_model(chn_name);
		cthis->mdl_observer.update(&cthis->mdl_observer, p_mdl);
	}
	
	self->hmi_run(self);
}

static void	DataHmi_HitHandle( HMI *self, char kcd)
{

	switch(kcd)
	{

			case KEYCODE_UP:
					 
					break;
			case KEYCODE_DOWN:
					
					break;
			case KEYCODE_LEFT:
					 
					break;
			case KEYCODE_RIGHT:
					 
					break;

			case KEYCODE_ENTER:
					self->switchHMI(self, g_p_HMI_menu, 0);
					break;		
			case KEYCODE_ESC:
					self->switchBack(self, 0);
					break;	
			
	}

//	if( !strcmp( s, KEYCODE_ENTER))
//	{
//		self->switchHMI(self, g_p_HMI_menu);
//		
//	}
//	if( !strcmp( s, HMIKEY_ESC))
//	{
//		self->switchBack(self, 0);
//	}
//	
}

static void DataHmi_Init_chnSht(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	int			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < phn_sys.sys_conf.num_chn; i++) {
		p_exp->inptSht( p_exp, (void *)datahmi_code_data, g_arr_p_chnData[i]) ;
		
		
		
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		
		
		
		
//		g_arr_p_chnData[i]->update = DataHmi_Data_update;
//		g_arr_p_chnUtil[i]->update = DataHmi_Util_update;
//		g_arr_p_chnAlarm[i]->update = DataHmi_Alarm_update;


	
	}
}







static int HDT_Update_mdl_chn_data(mdl_observer *self, void *p_srcMdl)
{
//	HMI_data		*cthis = SUB_PTR( self, mdl_observer, HMI_data);
//	HMI		*p_hmi = SUPER_PTR(cthis, HMI);
	Model	*p_mdl = (Model *)p_srcMdl;
	data_run_t *p_run = (data_run_t *)arr_p_vram[VRAM_RUN_NUM];
	short	chn_num = GET_MDL_CHN(p_mdl->mdl_id);
	
	
	
	Set_bit(p_run->set_need_update_channel, chn_num);
	p_run->arr_p_need_update_model[chn_num] = p_srcMdl;
	
//	if((p_hmi->flag & HMI_FLAG_HSA_SEM) == 0)
//	{
//		if(Sem_wait(&phn_sys.hmi_mgr.hmi_sem, 1000) <= 0) 
//			return ERR_RSU_BUSY;
//	
//	}
//	//更新实时值
//	DataHmi_Data_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	//更新单位
//	DataHmi_Util_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	//更新报警
//	DataHmi_Alarm_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	
//	//更新通道状态
//	if(phn_sys.sys_conf.show_chn_status)
//		DataHmi_Status_update(g_arr_p_chnData[chn_num], p_srcMdl);
//	
//	if((p_hmi->flag & HMI_FLAG_HSA_SEM) == 0)
//		Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	
	
	return RET_OK;
}
//两列三行
static int DataHmi_Data_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 30;
	uint8_t		right_x = 160;
	uint8_t		box_sizey = 70;		
	uint8_t		box_sizex = 160;		
	
	//到四周边界的空隙
	uint8_t		space_to_up = 		2;	
//	uint8_t		space_to_bottom = 	2;
//	uint8_t		space_to_left = 	8;	
	uint8_t		space_to_right = 	8;
	
	char 			row = 0, col = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	
	sheet			*p_sht = (sheet *)p_data;
	
		
//	if(IS_HMI_HIDE(g_p_dataHmi->flag))
//		return 0;
	
	row = p_sht->id / 2;
	col = p_sht->id % 2;

	p_sht->cnt.data = \
		p_mdl->to_string(p_mdl, AUX_DATA, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

	p_sht->area.x0 = right_x +  (col ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 = up_y + row * box_sizey + space_to_up;
	
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	Sheet_force_slide( p_sht);
	return 0;
	
	
	
}

static int DataHmi_Util_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 30;
	uint8_t		right_x = 160;
	uint8_t		box_sizey = 70;		
	uint8_t		box_sizex = 160;		
	
	//到四周边界的空隙
//	uint8_t		space_to_up = 		2;	
	uint8_t		space_to_bottom = 	4;
//	uint8_t		space_to_left = 	8;	
	uint8_t		space_to_right = 	4;
	
	char 			row = 0, col = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	
		
	
//	if(IS_HMI_HIDE(g_p_dataHmi->flag))
//		return 0;
	
	row = p_sht->id / 2;
	col = p_sht->id % 2;

	p_sht->cnt.data = \
		p_mdl->to_string(p_mdl, AUX_UNIT, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	

	p_sht->area.x0 = right_x +  (col ) * box_sizex - space_to_right - sizex;
	p_sht->area.y0 = up_y  + (row + 1) * box_sizey  -( sizey + space_to_bottom);
	
//	if(Sheet_is_hide(p_sht))
//		return 0;
	
	Sheet_force_slide( p_sht);
	return 0;
	
}
static int DataHmi_Alarm_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 30;
//	uint8_t		right_x = 160;
	uint8_t		box_sizey = 70;		
	uint8_t		box_sizex = 160;		
	
	//到四周边界的空隙
	uint8_t		space_to_up = 		20;	
//	uint8_t		space_to_bottom = 	2;
	uint8_t		space_to_left = 	4;	
//	uint8_t		space_to_right = 	0;
	
	char 			row = 0, col = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	char			old_font = p_sht->cnt.font;	

	
	row = p_sht->id / 2;
	col = p_sht->id % 2;

	p_sht->cnt.data = p_mdl->to_string(p_mdl, AUX_ALARM, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	
	p_sht->cnt.font = FONT_16;
	p_sht->p_gp->getSize( p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	
	p_sht->area.x0 = (col ) * box_sizex + space_to_left;
//	p_sht->area.y0 =  up_y  + (row + 1) * box_sizey  -( sizey + space_to_bottom);
	p_sht->area.y0 =  up_y  + (row) * box_sizey  + (space_to_up);
	

	Sheet_force_slide( p_sht);
	p_sht->cnt.font = old_font;
	return 0;
	
}

static int DataHmi_Status_update(void *p_data, Model *p_mdl)
{
	uint8_t		up_y = 30;
//	uint8_t		right_x = 160;
	uint8_t		box_sizey = 70;		
	uint8_t		box_sizex = 160;		
	
	//到四周边界的空隙
//	uint8_t		space_to_up = 		20;	
	uint8_t		space_to_bottom = 	8;
	uint8_t		space_to_left = 	4;	
//	uint8_t		space_to_right = 	8;
	
	char 			row = 0, col = 0;
	uint16_t 		sizex = 0;
	uint16_t 		sizey = 0;
	sheet			*p_sht = (sheet *)p_data;
	char			old_font = p_sht->cnt.font;
	
	row = p_sht->id / 2;
	col = p_sht->id % 2;

	
	
	p_sht->cnt.data = p_mdl->to_string(p_mdl, AUX_STATUS, NULL);
	p_sht->cnt.len = strlen( p_sht->cnt.data);
	
	p_sht->cnt.font = FONT_16;
	p_sht->p_gp->getSize(p_sht->p_gp, p_sht->cnt.font, &sizex, &sizey);
	sizex = sizex * p_sht->cnt.len;	
	p_sht->area.x0 = (col ) * box_sizex + space_to_left;
//	p_sht->area.y0 =  up_y  + (row) * box_sizey  + (space_to_up);
	p_sht->area.y0 =  up_y  + (row + 1) * box_sizey  -( sizey + space_to_bottom);
	

	Sheet_force_slide( p_sht);
	
	p_sht->cnt.font = old_font;
	return 0;
	
}


