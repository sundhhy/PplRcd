#include "HMI_main.h"
#include "HMIFactory.h"
#include "sdhDef.h"
#include "ExpFactory.h"
#include "format.h"
#include "chnInfoPic.h"
#include "focus.h"
#include "Component_Button.h"
#include "Component_curve.h"
#include "os/os_depend.h"
#include "Component_tips.h"

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

HMI *g_p_mainHmi;

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

//每个通道的单位
static ro_char MAIN_hmi_code_data[] = { "<text f=24 m=0 aux=0>100</>" };
static ro_char MAIN_hmi_code_unit[] = { "<text f=16 m=0 aux=1>m3/h</>" };
//通道报警:HH HI LI LL
static ro_char MAIN_hmi_code_alarm[] = { "<text f=16 m=0 aux=2> </>" };

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static mainHmi *signal_mainHmi;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	Init_mainHmi( HMI *self, void *arg);
static void	MainHmiShow( HMI *self);
static void MainHmiHide( HMI *self );
static void MaininitSheet( HMI *self );
static void HMI_MAIN_Run(HMI *self);

//static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total);


static void MainHmi_Init_chnShet(void);
static int MainHmi_Data_update(void *p_data, void *p_mdl);
static int MainHmi_Util_update(void *p_data, void *p_mdl);
static int MainHmi_Alarm_update(void *p_data, void *p_mdl);



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
mainHmi *Get_mainHmi(void) 
{
	if( signal_mainHmi == NULL)
	{
		signal_mainHmi = mainHmi_new();
		if(signal_mainHmi  == NULL) while(1);
		g_p_mainHmi = SUPER_PTR( signal_mainHmi, HMI);
	}
	
	return signal_mainHmi;
}

void Main_HMI_build_button(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	
	p->build_each_btn(0, BTN_TYPE_MENU, Main_btn_hdl, self);
	p->build_each_btn(1, BTN_TYPE_BAR, Main_btn_hdl, self);
	p->build_each_btn(2, BTN_TYPE_DIGITAL, Main_btn_hdl, self);
	p->build_each_btn(3, BTN_TYPE_TREND, Main_btn_hdl, self);
}

void	Main_HMI_hit( HMI *self, char *s)
{

	Button	*p = BTN_Get_Sington();

	if( !strcmp( s, HMIKEY_UP) )
	{

	}
	else if( !strcmp( s, HMIKEY_DOWN) )
	{
		
	}
	else if( !strcmp( s, HMIKEY_LEFT))
	{
		self->btn_backward(self);

	}
	else if( !strcmp( s, HMIKEY_RIGHT))
	{

		self->btn_forward(self);
	}
	
	
	
	if( !strcmp( s, HMIKEY_ENTER))
	{
		p->hit();
//		self->btn_hit(self);
	}
	if( !strcmp( s, HMIKEY_ESC))
	{
		self->switchBack(self);
	}
	
}

void Main_btn_hdl(void *arg, uint8_t btn_id)
{
	HMI					*self	= (HMI *)arg;		
	
	switch(btn_id)
	{
		
		case ICO_ID_MENU:
			self->switchHMI(self, g_p_HMI_menu);
			break;
		case ICO_ID_BAR:
			self->switchHMI(self, g_p_barGhHmi);
			break;	
		case ICO_ID_DIGITAL:
			self->switchHMI(self, g_p_dataHmi);
			break;
		case ICO_ID_TREND:
			self->switchHMI(self, g_p_RLT_trendHmi);
			break;	
			
	}

	
		
}


void Build_ChnSheets(void)
{
	int 		i = 0;
	shtctl 		*p_shtctl = NULL;
	Expr 		*p_exp ;
	Model			*p_mdl = NULL;
	char		mdl_code[16] = {0};
	p_shtctl = GetShtctl();
	
//	Bulid_ChnData(g_arr_p_chnData, (void *)MAIN_hmi_code_data, MainHmi_Data_update);
	
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		g_arr_p_chnData[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_data, g_arr_p_chnData[i]) ;
		sprintf(mdl_code,"chn_%d", i);

		g_arr_p_chnData[i]->p_mdl = ModelCreate(mdl_code);
		g_arr_p_chnData[i]->update = MainHmi_Data_update;
		g_arr_p_chnData[i]->id = i;
		p_mdl = g_arr_p_chnData[i]->p_mdl;
		p_mdl->attach(p_mdl, (Observer *)g_arr_p_chnData[i]);
		
		g_arr_p_chnUtil[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_unit, g_arr_p_chnUtil[i]) ;
		g_arr_p_chnUtil[i]->id = i;
		g_arr_p_chnUtil[i]->p_mdl = ModelCreate(mdl_code);
		g_arr_p_chnUtil[i]->update = MainHmi_Util_update;
		g_arr_p_chnUtil[i]->cnt.subType = TEXT_ST_UNTIL;
		p_mdl = g_arr_p_chnUtil[i]->p_mdl;
		p_mdl->attach(p_mdl, (Observer *)g_arr_p_chnUtil[i]);
		
		g_arr_p_chnAlarm[i] = Sheet_alloc( p_shtctl);
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_alarm, g_arr_p_chnAlarm[i]) ;
		g_arr_p_chnAlarm[i]->id = i;
		g_arr_p_chnAlarm[i]->p_mdl = ModelCreate(mdl_code);
		g_arr_p_chnAlarm[i]->update = MainHmi_Alarm_update;
		p_mdl = g_arr_p_chnAlarm[i]->p_mdl;
		p_mdl->attach(p_mdl, (Observer *)g_arr_p_chnAlarm[i]);
		
		
		
	}
	
}

CTOR( mainHmi)
SUPER_CTOR( HMI);
FUNCTION_SETTING( HMI.init, Init_mainHmi);
FUNCTION_SETTING( HMI.hide, MainHmiHide);
FUNCTION_SETTING( HMI.initSheet, MaininitSheet);
FUNCTION_SETTING( HMI.hmi_run, HMI_MAIN_Run);

FUNCTION_SETTING( HMI.show, MainHmiShow);
FUNCTION_SETTING( HMI.hitHandle, Main_HMI_hit);


FUNCTION_SETTING(HMI.build_component, Main_HMI_build_button);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int	Init_mainHmi( HMI *self, void *arg)
{
	HMI 					*p_cmm;
	Button					*p_btn;
	Progress_bar			*p_bar;
	Curve					*p_crv;
	CMP_tips				*p_tips;

	
	p_cmm = CreateHMI( HMI_CMM);
	p_cmm->init( p_cmm, NULL);
	self->flag = 0;
	//初始化界面组件
	p_btn = BTN_Get_Sington();
	p_btn->init(p_btn);
	p_bar = PGB_Get_Sington();
	p_bar->init(p_bar);
	p_crv = CRV_Get_Sington();
	p_crv->init(p_crv, NUM_CHANNEL);
	p_tips = TIP_Get_Sington();
	p_tips->init(p_tips);
	
	self->initSheet( self);

	phn_sys.hmi_mgr.hmi_sem = Alloc_sem();
	Sem_init(&phn_sys.hmi_mgr.hmi_sem);
	Sem_post(&phn_sys.hmi_mgr.hmi_sem);
	return RET_OK;
	

}


static void HMI_MAIN_Run(HMI *self)
{
	
	
	
}



//static void BuildChnInfoPic( sheet *arr_p_sheets[ CHN_ROW + 2][CHN_COL], char total)
//{
//	char count, i, j, numCol;
//	char arr_col[8] = { 1, 2, 2, 2, 3, 3, 3, 4};
//	
//	numCol = arr_col[ total - 1];
//	
//	
//	count = 0;
//	for( i = 1; i < 2 + 1; i++) {
//		for( j = 0; j < numCol; j ++) {
//			
//			arr_p_sheets[i][j] = CIF_build_pic( count++, total);
//		}
//		
//	}
//	
//}
static void MainHmiHide( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
//	int i;
	

	
//	Sheet_updown(g_p_ico_trend, -1);
//	Sheet_updown(g_p_ico_digital, -1);
//	Sheet_updown(g_p_ico_bar, -1);
//	Sheet_updown(g_p_ico_memu, -1);
//	for( i = 0; i < NUM_CHANNEL; i++) {		//
//		
//		Sheet_updown(g_arr_p_chnAlarm[i], -1);
//		Sheet_updown(g_arr_p_chnUtil[i], -1);
//		Sheet_updown(g_arr_p_chnData[i], -1);
//	}
	Sheet_updown(g_p_shtTime, -1);
	Sheet_updown(g_p_sht_title, -1);
	Sheet_updown(g_p_sht_bkpic, -1);
	self->clear_focus(self, self->p_fcuu->focus_row, self->p_fcuu->focus_col);
//	Focus_free(self->p_fcuu);
	
}	

static void MaininitSheet( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
	int h = 0;;
	
	h = 0;
	g_p_sht_bkpic->cnt.data = MAINHMI_BKPICNUM;

	g_p_sht_title->cnt.data = MAINHMI_TITLE;
	g_p_sht_title->cnt.len = strlen(MAINHMI_TITLE);
	
	Sheet_updown(g_p_sht_bkpic, h++);
	Sheet_updown(g_p_sht_title, h++);
	Sheet_updown(g_p_shtTime, h++);
//	for( i = 0; i < NUM_CHANNEL; i++) {
//		Sheet_updown(g_arr_p_chnData[i], h++);
//		Sheet_updown(g_arr_p_chnUtil[i], h++);
//		Sheet_updown(g_arr_p_chnAlarm[i], h++);
//		
//	}

//	Sheet_updown(g_p_ico_memu, h++);
//	Sheet_updown(g_p_ico_bar, h++);
//	Sheet_updown(g_p_ico_digital, h++);
//	Sheet_updown(g_p_ico_trend, h++);
	
	
	MainHmi_Init_chnShet();
	self->init_focus(self);
	
}


static void	MainHmiShow( HMI *self )
{
//	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
//	int		i = 0;
//	I_dev_lcd 	*p_lcd;
	g_p_curHmi = self;
//	Dev_open( LCD_DEVID, (void *)&p_lcd);
//	p_lcd->Clear( mainHmiAtt.bkc);
//	MainHmi_Init_chnShet();
	
	Sheet_refresh(g_p_sht_bkpic);
//	Sheet_refresh( cthis->p_title);
//	self->show_focus( self, 0, 0);
}






static void MainHmi_Init_chnShet(void)
{
	Expr 		*p_exp ;
//	Model		*p_mdl = NULL;
	int			i = 0;
	p_exp = ExpCreate( "text");
	for(i = 0; i < NUM_CHANNEL; i++) {
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_data, g_arr_p_chnData[i]) ;
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_unit, g_arr_p_chnUtil[i]) ;
		p_exp->inptSht( p_exp, (void *)MAIN_hmi_code_alarm, g_arr_p_chnAlarm[i]) ;
		
		g_arr_p_chnData[i]->id = i;
		g_arr_p_chnUtil[i]->id = i;
		g_arr_p_chnAlarm[i]->id = i;
		
		g_arr_p_chnData[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnUtil[i]->cnt.colour = arr_clrs[i];
		g_arr_p_chnAlarm[i]->cnt.colour = arr_clrs[i];
		
		
		
		g_arr_p_chnData[i]->update = MainHmi_Data_update;
		g_arr_p_chnUtil[i]->update = MainHmi_Util_update;
		g_arr_p_chnAlarm[i]->update = MainHmi_Alarm_update;

		//这是为了初始化的时候，就能让数据得到正确的坐标
//		g_arr_p_chnData[i]->update(g_arr_p_chnData[i], NULL);
//		g_arr_p_chnAlarm[i]->update(g_arr_p_chnAlarm[i], NULL);
//		g_arr_p_chnUtil[i]->update(g_arr_p_chnUtil[i], NULL);
	
	}
}
//static void MainHmi_ClearFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
////	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
////	sheet  			*p_fouse = arr_p_focus[ fouse_col];
////	
////	p_fouse->cnt.effects = GP_CLR_EFF( p_fouse->cnt.effects, EFF_FOCUS);
////	Sheet_slide( p_fouse);
//	
////	sheet *p_sht = Focus_Get_sht(self->p_fcuu, fouse_row, fouse_col);
////	
////	if(p_sht == NULL)
////		return;
////	p_sht->cnt.effects = GP_CLR_EFF( p_sht->cnt.effects, EFF_FOCUS);
////	Sheet_slide( p_sht);
//}
//static void MainHmi_ShowFocuse( HMI *self, uint8_t fouse_row, uint8_t fouse_col)
//{
////	mainHmi		*cthis = SUB_PTR( self, HMI, mainHmi);
////	sheet  			*p_fouse = arr_p_focus[ fouse_col];
////	
////	p_fouse->cnt.effects = GP_SET_EFF( p_fouse->cnt.effects, EFF_FOCUS);
////	Sheet_slide( p_fouse);
//}

static int MainHmi_Data_update(void *p_data, void *p_mdl)
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
	
	if(IS_HMI_HIDE(g_p_mainHmi->flag))
		return 0;

	p_sht->cnt.data = \
		p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
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

static int MainHmi_Util_update(void *p_data, void *p_mdl)
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
	
		
	if(IS_HMI_HIDE(g_p_mainHmi->flag))
		return 0;
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
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

static int MainHmi_Alarm_update(void *p_data, void *p_mdl)
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
	
		
	if(IS_HMI_HIDE(g_p_mainHmi->flag))
		return 0;
	i = p_sht->id % 3;
	j = p_sht->id / 3;

	p_sht->cnt.data = \
		p_sht->p_mdl->to_string( p_sht->p_mdl, p_sht->cnt.mdl_aux, NULL);
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






