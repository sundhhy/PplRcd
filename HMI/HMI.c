#include "HMI.h"
#include "commHMI.h"
#include <string.h>
//提供 按键，事件，消息，窗口，报警，时间，复选框的图层
//这些图层可能会被其他界面所使用
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
const Except_T Hmi_Failed = { "HMI Failed" };


//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

	
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
HMI *g_p_curHmi;
HMI *g_p_lastHmi;
HMI *g_p_win_last;
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
static void	SwitchHMI( HMI *self, HMI *p_hmi);
static void	SwitchBack( HMI *self);
static void HitHandle( HMI *self, char *s_key);
static void LngpshHandle( HMI *self, char *s_key);
static void DHitHandle( HMI *self, char *s_key);
static void ConposeKeyHandle(HMI *self, char *s_key1, char *s_key2);

void	Init_focus(HMI *self);
void	Clear_focus(HMI *self, uint8_t fouse_row, uint8_t fouse_col);
void	Show_focus( HMI *self, uint8_t fouse_row, uint8_t fouse_col);

static void		HMI_Build_button(HMI *self);
static void		HMI_Clean_button(HMI *self);
static void		HMI_Show_button(HMI *self);
static int		HMI_Btn_forward(HMI *self);
static int		HMI_Btn_backward(HMI *self);
static void		HMI_Btn_jumpout(HMI *self);
//static void		HMI_Btn_hit(HMI *self);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
void Set_flag_show(uint8_t	*p_flag, int val)
{
	val &= 1;
	*p_flag &= 0xfe;
	*p_flag |= val;
}

void Set_flag_keyhandle(uint8_t	*p_flag, int val)
{
	val &= 2;
	*p_flag &= 0xfd;
	*p_flag |= val;
}






ABS_CTOR( HMI)
FUNCTION_SETTING( show, HmiShow);
FUNCTION_SETTING( switchHMI, SwitchHMI);
FUNCTION_SETTING( switchBack, SwitchBack);
FUNCTION_SETTING( hitHandle, HitHandle);
FUNCTION_SETTING( longpushHandle, LngpshHandle);
FUNCTION_SETTING( dhitHandle, DHitHandle);
FUNCTION_SETTING( conposeKeyHandle, ConposeKeyHandle);


FUNCTION_SETTING( init_focus, Init_focus);
FUNCTION_SETTING( clear_focus, Clear_focus);
FUNCTION_SETTING( show_focus, Show_focus);

FUNCTION_SETTING(build_component, HMI_Build_button);
FUNCTION_SETTING(clean_button, HMI_Clean_button);
FUNCTION_SETTING(show_button, HMI_Show_button);
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




static void	HmiShow( HMI *self)
{
	
	
}


static void	SwitchHMI( HMI *self, HMI *p_hmi)
{
	if( p_hmi == NULL)
		return;
	if(p_hmi ==  g_p_winHmi ) {
		
		g_p_win_last = self;
	} else if((self != p_hmi) && (self != g_p_winHmi)) {		//切换到不同的界面上，才更新
		
		g_p_lastHmi = g_p_curHmi;
		
	}
	phn_sys.key_weight = 1;
	
	g_p_curHmi = p_hmi;
	Set_flag_show(&self->flag, 0);
	self->hide(self);
	self->clean_button(self);
	
	p_hmi->initSheet( p_hmi);
	p_hmi->build_component(p_hmi);
	p_hmi->show( p_hmi);
	p_hmi->show_button(p_hmi);
	
	Set_flag_show(&p_hmi->flag, 1);
	
}

static void	SwitchBack( HMI *self)
{
	HMI *nowHmi = g_p_lastHmi;
	if(g_p_lastHmi == NULL)
		return;
	
	g_p_lastHmi = g_p_curHmi;
	g_p_curHmi = nowHmi;
	
	phn_sys.key_weight = 1;
	Set_flag_show(&self->flag, 0);
	self->hide( self);
	self->clean_button(self);
	
	
	nowHmi->initSheet( nowHmi);
	nowHmi->build_component(nowHmi);
	nowHmi->show( nowHmi);
	nowHmi->show_button(nowHmi);
	Set_flag_show(&nowHmi->flag, 1);
	
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
	} else if( !strcmp( s_key1, HMIKEY_RIGHT) && !strcmp( s_key2, HMIKEY_LEFT)) {
		phn_sys.sys_flag |= SYSFLAG_SETTING;
		self->switchHMI(self, g_p_Setup_HMI);
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


static void		HMI_Build_button(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	int		i;
	for(i = 0; i < NUM_BUTTON; i++)
	{
		p->build_each_btn(i, BTN_TYPE_NONE, NULL, NULL);


	}

}

static void		HMI_Clean_button(HMI *self)
{

	Button	*p = BTN_Get_Sington();
	p->clean_btn();
	self->flag &= ~HMIFLAG_FOCUS_IN_BTN;
}
static void		HMI_Show_button(HMI *self)
{
	Button	*p = BTN_Get_Sington();
	
	p->show_vaild_btn();

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



