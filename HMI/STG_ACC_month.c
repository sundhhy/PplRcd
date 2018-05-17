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

static int SAM_Entry(int row, int col, void *pp_text);
static int SAM_Init(void *arg);
static void SAM_Build_component(void *arg);
static int SAM_Key_UP(void *arg);
static int SAM_Key_DN(void *arg);
static int SAM_Key_LT(void *arg);
static int SAM_Key_RT(void *arg);
static int SAM_Key_ET(void *arg);
static int SAM_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf);
static int SAM_Commit(void *arg);
static void SAM_Exit(void);

strategy_t	g_st_acc_month = {
	SAM_Entry,
	SAM_Init,
	SAM_Build_component,
	SAM_Key_UP,
	SAM_Key_DN,
	SAM_Key_LT,
	SAM_Key_RT,
	SAM_Key_ET,
	SAM_Get_focus_data,	
	SAM_Commit,
	SAM_Exit,
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
#define STG_SELF		g_st_acc_month

#define SAM_RAM_ACC_SDATA				0
#define SAM_RAM_ACC_SCHN				1
#define SAM_RAM_SDATE					2
#define SAM_RAM_SDAY					3
#define SAM_RAM_SCHN_NUM				4
#define SAM_RAM_UNIT						5
#define SAM_RAM_MAX							6

#define STG_RUN_VRAM_NUM			6		//运行内存单独分配
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	int cur_chn;
	
	
}sam_run_t;

#define STG_P_RUN		(sam_run_t *)arr_p_vram[STG_RUN_VRAM_NUM]
#define INIT_RUN_RAM arr_p_vram[STG_RUN_VRAM_NUM] = HMI_Ram_alloc(sizeof(sam_run_t))
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void SAM_Print_ACC(char m, char type, char *s);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int SAM_Entry(int row, int col, void *pp_text)
{
	Model_chn				*p_mc;
	Model						*p_md;
	
	
	char 						**pp = (char **)pp_text;
	short						r = 0, pic_num = 0;
	char						month = 0;
	sam_run_t				*p_run = STG_P_RUN;


	
	
	
	p_mc = Get_Mode_chn(p_run->cur_chn);
	p_md = SUPER_PTR(p_mc, Model);
	
	
	if(col > 3)		
		return 0;
	
	
	
	r = row % STRIPE_MAX_ROWS;		//条纹界面上的行数是11
	pic_num = row / STRIPE_MAX_ROWS + 1;  //第几副画面
	//计算当前行 列下对应的日子
	if(row > 0)
		month = row - pic_num ;		
	if(month > 13)
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
				*pp = "年份";
				break;
			case 1:
				sprintf(arr_p_vram[SAM_RAM_SDATE], "%02d", arr_chn_acc[p_run->cur_chn].sum_year);
				*pp = arr_p_vram[SAM_RAM_SDATE];
				break;
			case 2:
				*pp = "通道";
				break;
			case 3:
				sprintf(arr_p_vram[SAM_RAM_ACC_SCHN], "%d", p_run->cur_chn);
				*pp = arr_p_vram[SAM_RAM_ACC_SCHN];
				break;
			
			
		}
		
		
	}
	else if(r == 1)
	{
		switch(col)
		{
			case 0:
				SAM_Print_ACC(month, SAM_RAM_SDAY, arr_p_vram[SAM_RAM_SDAY]);
				*pp = arr_p_vram[SAM_RAM_SDAY];
				break;
			case 1:
				SAM_Print_ACC(month, SAM_RAM_ACC_SDATA, arr_p_vram[SAM_RAM_ACC_SDATA]);
				*pp = arr_p_vram[SAM_RAM_ACC_SDATA];
				break;
			case 2:
				*pp = "单位";
				break;
			case 3:
				p_md->to_string(p_md, AUX_UNIT, arr_p_vram[SAM_RAM_UNIT]);
				*pp = arr_p_vram[SAM_RAM_UNIT];
				break;
			
		}
		
	}
	else 
	{
		

		
		//显示：日 日累积
		if(col == 0)
		{

			SAM_Print_ACC(month, SAM_RAM_SDAY, arr_p_vram[SAM_RAM_SDAY]);
			*pp = arr_p_vram[SAM_RAM_SDAY];
			
		}
		else 
		{
			SAM_Print_ACC(month, SAM_RAM_ACC_SDATA, arr_p_vram[SAM_RAM_ACC_SDATA]);
			*pp = arr_p_vram[SAM_RAM_ACC_SDATA];
		}
	}
	

	return strlen(*pp);		
}

//调用者保证m小于14
static void SAM_Print_ACC(char m, char type, char *s)
{
	sam_run_t				*p_run = STG_P_RUN;
	
	if(m < 12)
	{
		if(type ==  SAM_RAM_ACC_SDATA)
			CNA_Print_acc_val(arr_chn_acc[p_run->cur_chn].accumlated_month[m], s, 1);
		else 
			sprintf(s, "%02d", m + 1);
	}
	else if(m == 12)
	{
		
		if(type ==  SAM_RAM_ACC_SDATA)
			CNA_Print_acc_val(arr_chn_acc[p_run->cur_chn].accumlated_year, s, 1);
		else 
			sprintf(s, "年累计");
		
	}
	else if(m == 13)
	{
		if(type ==  SAM_RAM_ACC_SDATA)
			CNA_Print_acc_val(arr_chn_acc[p_run->cur_chn].accumlated_total, s, 1);
		else 
			sprintf(s, "总累计");
		
		
	}

	
}

static int SAM_Init(void *arg)
{
	int i = 0;
	sam_run_t				*p_run = STG_P_RUN;
	
	HMI_Ram_init();
	for(i = 0; i < SAM_RAM_MAX; i++) {
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	INIT_RUN_RAM;
	
	STG_SELF.total_col = 4;
	STG_SELF.total_row = 14;
	p_run->cur_chn = 0;
	
	return RET_OK;
}

static void SAM_Build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();

	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
		
}
static void SAM_Exit(void)
{
	
}
static int SAM_Commit(void *arg)
{
	return RET_OK;
	
}

static int SAM_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf)
{

	

	return 0;
	
}

static int SAM_Key_UP(void *arg)
{
	sam_run_t				*p_run = STG_P_RUN;
	p_run->cur_chn ++;
	if(p_run->cur_chn == phn_sys.sys_conf.num_chn)
		p_run->cur_chn = 0;
	
	STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush, NULL);
	
	return -1;
}
static int SAM_Key_DN(void *arg)
{
	sam_run_t				*p_run = STG_P_RUN;
	if(p_run->cur_chn)
		p_run->cur_chn --;
	else
		p_run->cur_chn = phn_sys.sys_conf.num_chn - 1;
	STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush, NULL);
	return -1;
}
static int SAM_Key_LT(void *arg)
{
	
	return -1;
}

static int SAM_Key_RT(void *arg)
{
	
	return -1;
}
static int SAM_Key_ET(void *arg)
{
	return -1;
	
}






