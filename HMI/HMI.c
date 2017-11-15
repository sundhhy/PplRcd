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

//ro_char str_endRow[] = "</row>" ;
//ro_char str_endCol[] = "</col>" ;
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
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
//int Is_rowEnd( const char *str)
//{
//	if( strcmp( str, str_endRow) == 0)
//		return 1;
//	
//	return 0;
//	
//}
//int Is_colEnd( const char *str)
//{
//	if( strcmp( str, str_endCol) == 0)
//		return 1;
//	
//	return 0;
//	
//}


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
	
	g_p_curHmi = p_hmi;
	Set_flag_show(&self->flag, 0);
	self->hide(self);
	p_hmi->initSheet( p_hmi);
	p_hmi->show( p_hmi);

	Set_flag_show(&p_hmi->flag, 1);
	
}

static void	SwitchBack( HMI *self)
{
	HMI *nowHmi = g_p_lastHmi;
	g_p_lastHmi = g_p_curHmi;
	g_p_curHmi = nowHmi;
	Set_flag_show(&self->flag, 0);
	self->hide( self);
	nowHmi->initSheet( nowHmi);
	nowHmi->show( nowHmi);
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

		
		self->switchHMI(self, g_p_Setup_HMI);
	} else if( !strcmp( s_key1, HMIKEY_RIGHT) && !strcmp( s_key2, HMIKEY_LEFT)) {
		
		self->switchHMI(self, g_p_Setup_HMI);
	}
	
	
}

void	Init_focus(HMI *self) {}
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




