//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "Component_tips.h"
#include "ExpFactory.h"
#include "HMI.h"
#include "sdhDef.h"
#include "arithmetic/bit.h"
//#include "basis/assert.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define TIP_CLEAN_ICO		"33"

#define	TIP_USB_NUM				31
#define	TIP_ALARM_NUM			32
#define	TIP_ERR_NUM				35
#define	TIP_ERASE_NUM			36
//4个按钮的图形代码
static ro_char *arr_tips_code[NUM_TIP_ICO] ={ \
	"<pic vx0=120  vy0=0 >0</>" , \
	"<pic  vx0=152  vy0=0 >0</>" ,\
	"<pic  vx0=184  vy0=0 >0</>" ,\
	
};

//static const char TIP_code_box[] = { "<box ></>" };
//static const char TIP_code_tip_text[] = { "<text f=16 m=0 >0</>" };
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


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
//typedef void (*cal_end_by_val)(sheet *p, uint16_t len, uint8_t prc);
////两个实心方框和一个文字提示符组成一个进度条
//typedef struct {
//	uint8_t			toward;
//	uint8_t			bar_val;
//	uint16_t		bar_len;
//	char			text_buf[4];
//	sheet			*p_border;
//	sheet			*p_shade;
//	sheet			*p_bar;
//	sheet			*p_tip_text;
//}TIP_t;
		
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static CMP_tips 			*p_TIP_self = NULL;
static sheet				*arr_p_tip_ico[NUM_TIP_ICO];
static char					arr_s_ico[NUM_TIP_ICO][3];
//static cal_end_by_val		arr_cal[2];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	TIP_Init(CMP_tips *self);
static void		TIP_Show_ico_tips(uint8_t tips_seq, short pic_num);
static void		TIP_Clear_ico_tips(uint8_t tips_seq);
static void		TIP_Hide_ico_tips(char ctl);
static void		TIP_Show_tips();
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
CMP_tips	*TIP_Get_Sington(void)
{
	
	if(p_TIP_self == NULL)
	{
		p_TIP_self = CMP_tips_new();
		if(p_TIP_self  == NULL) while(1);
	}
	
	return p_TIP_self;
	
}


CTOR(CMP_tips)
FUNCTION_SETTING(init, TIP_Init);
FUNCTION_SETTING(show_ico_tips, TIP_Show_ico_tips);
FUNCTION_SETTING(clear_ico_tips, TIP_Clear_ico_tips);
FUNCTION_SETTING(hide_ico_tips, TIP_Hide_ico_tips);
FUNCTION_SETTING(show_tips, TIP_Show_tips);
//FUNCTION_SETTING(show_bar, TIP_Show_bar);

END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void 	TIP_Init(CMP_tips *self)
{


	shtctl 		*p_shtctl = NULL;
	Expr 			*p_exp ;
	int				i;
//		
	p_shtctl = GetShtctl();
//	arr_cal[0] = TIP_Cal_cross;
//	arr_cal[1] = TIP_Cal_paraller;
//	

	for(i = 0; i < NUM_TIP_ICO; i++)
	{
		p_exp = ExpCreate("pic");
		arr_p_tip_ico[i] = Sheet_alloc( p_shtctl);
		
		p_exp->inptSht(p_exp, (void *)arr_tips_code[i], arr_p_tip_ico[i]);
		
	}
//	
	self->flag_hide_tips = 0;
	self->set_ico_vaild_tip = 0;


//	p_TIP_self = self;
	
	
}

static void		TIP_Show_ico_tips(uint8_t tips_seq, short pic_num)
{
	
	if(tips_seq > NUM_TIP_ICO)
		return;
	
	if(pic_num >= 0)
		sprintf(arr_s_ico[tips_seq], "%d", pic_num);
	else
	{
		if(tips_seq == 0)
			sprintf(arr_s_ico[tips_seq], "%d", TIP_USB_NUM);
		else if(tips_seq == 1)
			sprintf(arr_s_ico[tips_seq], "%d", TIP_ALARM_NUM);
		else if(tips_seq == 2)
			sprintf(arr_s_ico[tips_seq], "%d", TIP_ERR_NUM);
		
		
	}
	
//	arr_p_tip_ico[tips_seq]->e_heifht = 1;
	arr_p_tip_ico[tips_seq]->cnt.data = arr_s_ico[tips_seq];
	Sheet_force_slide(arr_p_tip_ico[tips_seq]);
//	arr_p_tip_ico[tips_seq]->e_heifht = 0;
	
	
	Set_bit(&p_TIP_self->set_ico_vaild_tip, tips_seq);
	p_TIP_self->flag_hide_tips = 0;
}
static void		TIP_Clear_ico_tips(uint8_t tips_seq)
{
//	char	pic_num[] = "32";
	
	if(tips_seq > NUM_TIP_ICO)
		return;
	if(Check_bit(&p_TIP_self->set_ico_vaild_tip, tips_seq) == 0)
		return;
	
	
	
//	arr_p_tip_ico[tips_seq]->e_heifht = 1;
	arr_p_tip_ico[tips_seq]->cnt.data = TIP_CLEAN_ICO;
	Sheet_force_slide(arr_p_tip_ico[tips_seq]);
//	arr_p_tip_ico[tips_seq]->e_heifht = 0;
	
//	Set_bit(&p_TIP_self->set_ico_hide_tip, tips_seq);
	Clear_bit(&p_TIP_self->set_ico_vaild_tip, tips_seq);
	arr_p_tip_ico[tips_seq]->cnt.data = arr_s_ico[tips_seq];		//恢复
	
}

static void		TIP_Hide_ico_tips(char ctl)
{
	
		
	p_TIP_self->flag_hide_tips = ctl;
	
	
	
}

static void		TIP_Show_tips()
{
	int i;
	
	if(p_TIP_self->flag_hide_tips)
		return;
	
	for(i = 0; i < NUM_TIP_ICO; i++)
	{
		if(Check_bit(&p_TIP_self->set_ico_vaild_tip, i) == 0)
			continue;
		
//		arr_p_tip_ico[i]->e_heifht = 1;
		Sheet_force_slide(arr_p_tip_ico[i]);
//		arr_p_tip_ico[i]->e_heifht = 0;
			
		
		
		
	}
	
}




