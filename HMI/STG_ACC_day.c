#include <stdint.h>
#include "HMI_striped_background.h"
#include "utils/Storage.h"
#include "os/os_depend.h"
#include "channel_accumulated.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static int SAD_Entry(int row, int col, void *pp_text);
static int SAD_Init(void *arg);
static void SAD_Build_component(void *arg);
static int SAD_Key_UP(void *arg);
static int SAD_Key_DN(void *arg);
static int SAD_Key_LT(void *arg);
static int SAD_Key_RT(void *arg);
static int SAD_Key_ET(void *arg);
static int SAD_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf);
static int SAD_Commit(void *arg);
static void SAD_Exit(void);

strategy_t	g_st_acc_day = {
	SAD_Entry,
	SAD_Init,
	SAD_Build_component,
	SAD_Key_UP,
	SAD_Key_DN,
	SAD_Key_LT,
	SAD_Key_RT,
	SAD_Key_ET,
	SAD_Get_focus_data,	
	SAD_Commit,
	SAD_Exit,
};



//static int Cns_init(void *arg);
//static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
//strategy_t	g_chn_strategy = {
//	ChnStrategy_entry,
//	Cns_init,
//	Cns_key_up,
//	Cns_key_dn,
//	Cns_key_lt,
//	Cns_key_rt,
//	Cns_key_er,
//	Cns_get_focusdata,
//};
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define STG_SELF		g_st_acc_day

#define SAD_RAM_ACC_SDATA				0
#define SAD_RAM_ACC_SCHN				1
#define SAD_RAM_SDATE					2
#define SAD_RAM_SDAY					3
#define SAD_RAM_SCHN_NUM				4
#define SAD_RAM_UNIT						5
#define SAD_RAM_MAX							6

#define STG_RUN_VRAM_NUM			6		//运行内存单独分配
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	int cur_chn;
	
	
}sad_run_t;

#define STG_P_RUN		(sad_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
#define INIT_RUN_RAM arr_p_vram[STG_RUN_VRAM_NUM] = HMI_Ram_alloc(sizeof(sad_run_t))
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int SAD_Entry(int row, int col, void *pp_text)
{

	
	char 						**pp = (char **)pp_text;
	short						r = 0, pic_num = 0;
	char						day = 0;
	
	sad_run_t				*p_run;
	Model_chn				*p_mc;
	Model						*p_md;
	

	p_run = STG_P_RUN;
	p_mc = Get_Mode_chn(p_run->cur_chn);
	p_md = SUPER_PTR(p_mc, Model);

	if(col > 3)		//最多4列
		return 0;
	
	r = row % STRIPE_MAX_ROWS;		//条纹界面上的行数是11
	pic_num = row / STRIPE_MAX_ROWS + 1;  //第几副画面
	//计算当前行 列下对应的日子
	if(row > 0)
		day = row - pic_num ;		
	if(day > 30)
		return 0;
	if((r > 1) && (col > 1))
	{
		//第0，1行有4列
		//其他行只有2列
		return 0;
		
	}
	
	
	
	
	
	if(r == 0)
	{
		//显示累积的月份与通道号
		switch(col)
		{
			case 0:
				*pp = "月份";
				break;
			case 1:
				sprintf(arr_p_vram[SAD_RAM_SDATE], "%02d.%02d", arr_chn_acc[p_run->cur_chn].sum_year, arr_chn_acc[p_run->cur_chn].sum_month);
				*pp = arr_p_vram[SAD_RAM_SDATE];
				break;
			case 2:
				*pp = "通道";
				break;
			case 3:
				sprintf(arr_p_vram[SAD_RAM_ACC_SCHN], "%d", p_run->cur_chn);
				*pp = arr_p_vram[SAD_RAM_ACC_SCHN];
				break;
			
			
		}
		
		
	}
	else if(r == 1)
	{
		
		
		//显示累积的月份与通道号
		switch(col)
		{
			case 0:
				sprintf(arr_p_vram[SAD_RAM_SDAY], "%02d", day + 1);
				*pp = arr_p_vram[SAD_RAM_SDAY];
				break;
			case 1:
				CNA_Print_acc_val(arr_chn_acc[p_run->cur_chn].accumlated_day[day], arr_p_vram[SAD_RAM_ACC_SDATA], 1);
				*pp = arr_p_vram[SAD_RAM_ACC_SDATA];
				break;
			case 2:
				*pp = "单位";
				break;
			case 3:
				p_md->to_string(p_md, AUX_UNIT, arr_p_vram[SAD_RAM_UNIT]);
				*pp = arr_p_vram[SAD_RAM_UNIT];
				break;
			
		}
		
	}
	else 
	{
		

		
		//显示：日 日累积
		if(col == 0)
		{
			//显示日子
			sprintf(arr_p_vram[SAD_RAM_SDAY], "%02d", day + 1);
			*pp = arr_p_vram[SAD_RAM_SDAY];
			
			
		}
		else 
		{
			
//			acc_val = CNA_arr_u16_2_u64(arr_chn_acc[p_run->cur_chn].accumlated_day[day], 3);
//			sprintf(arr_p_vram[SAD_RAM_ACC_SDATA], "%12d.%d", acc_val/10, acc_val % 10);
			CNA_Print_acc_val(arr_chn_acc[p_run->cur_chn].accumlated_day[day], arr_p_vram[SAD_RAM_ACC_SDATA], 1);
			*pp = arr_p_vram[SAD_RAM_ACC_SDATA];
		}
	}
	

	return strlen(*pp);
		
}

static int SAD_Init(void *arg)
{
	int i = 0;
	sad_run_t *p_run;
	
	HMI_Ram_init();
	for(i = 0; i < SAD_RAM_MAX; i++) {
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	INIT_RUN_RAM;
	STG_SELF.total_col = 4;
	STG_SELF.total_row = 31;
	p_run = STG_P_RUN;
	p_run->cur_chn = 0;
	
	return RET_OK;
}

static void SAD_Build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();

	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
		
}
static void SAD_Exit(void)
{
	
}
static int SAD_Commit(void *arg)
{
	return RET_OK;
	
}

static int SAD_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf)
{

	

	return 0;
	
}

static int SAD_Key_UP(void *arg)
{
		
	sad_run_t				*p_run = STG_P_RUN;

	p_run->cur_chn ++;
	if(p_run->cur_chn == phn_sys.sys_conf.num_chn)
		p_run->cur_chn = 0;
	
	STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush, NULL);
	
	return -1;
}
static int SAD_Key_DN(void *arg)
{
		
	sad_run_t				*p_run = STG_P_RUN;

	
	if(p_run->cur_chn)
		p_run->cur_chn --;
	else
		p_run->cur_chn = phn_sys.sys_conf.num_chn - 1;
	STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush, NULL);
	return -1;
}
static int SAD_Key_LT(void *arg)
{
	
	return -1;
}

static int SAD_Key_RT(void *arg)
{
	
	return -1;
}
static int SAD_Key_ET(void *arg)
{
	return -1;
	
}






