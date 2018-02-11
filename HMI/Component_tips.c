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


//4个按钮的图形代码
static ro_char *arr_tips_code[NUM_TIP_ICO] ={ \
	"<pic vx0=120  vy0=0 >29</>" , \
	"<pic  vx0=120  vy0=0 >30</>" ,\
	
};

static const char TIP_code_box[] = { "<box ></>" };
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
//static cal_end_by_val		arr_cal[2];
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void 	TIP_Init(CMP_tips *self);
static void 	TIP_Set_self(CMP_tips *self);

static void		TIP_Show_tips(uint8_t tips_type, uint8_t tips_seq, short pic_num);
static void		TIP_Clear_tips(uint8_t tips_type, uint8_t tips_seq);

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
FUNCTION_SETTING(show_tips, TIP_Show_tips);
FUNCTION_SETTING(clear_tips, TIP_Clear_tips);
//FUNCTION_SETTING(update_bar, TIP_Update_bar);
//FUNCTION_SETTING(delete_bar, TIP_Delete_bar);
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
//	self->set_free_bar = 0xff;


//	p_TIP_self = self;
	
	
}

static void		TIP_Show_tips(uint8_t tips_type, uint8_t tips_seq, short pic_num)
{
	char	s_pic_num[] = "29";
	arr_p_tip_ico[tips_seq]->e_heifht = 1;
	arr_p_tip_ico[tips_seq]->cnt.data = s_pic_num;
	Sheet_slide(arr_p_tip_ico[tips_seq]);
	arr_p_tip_ico[tips_seq]->e_heifht = 0;
	
	
}
static void		TIP_Clear_tips(uint8_t tips_type, uint8_t tips_seq)
{
	char	pic_num[] = "30";
	arr_p_tip_ico[tips_seq]->e_heifht = 1;
	arr_p_tip_ico[tips_seq]->cnt.data = pic_num;
	Sheet_slide(arr_p_tip_ico[tips_seq]);
	arr_p_tip_ico[tips_seq]->e_heifht = 0;
	
	
}




