#include <stdint.h>
#include "HMI_striped_background.h"
#include "utils/Storage.h"
#include "utils/log.h"
#include "HMI_windows.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static int NPW_Entry(int row, int col, void *pp_text);
static int NPW_Init(void *arg);
static void NPW_Build_component(void *arg);
static int NPW_Key_UP(void *arg);
static int NPW_Key_DN(void *arg);
static int NPW_Key_LT(void *arg);
static int NPW_Key_RT(void *arg);
static int NPW_Key_ET(void *arg);
static int NPW_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf);
static int NPW_Commit(void *arg);
static void NPW_Exit(void);

strategy_t	g_news_power = {
	NPW_Entry,
	NPW_Init,
	NPW_Build_component,
	NPW_Key_UP,
	NPW_Key_DN,
	NPW_Key_LT,
	NPW_Key_RT,
	NPW_Key_ET,
	NPW_Get_focus_data,	
	NPW_Commit,
	NPW_Exit,
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
#define STG_SELF  g_news_power

#define NUM_ROW		11

#define STG_NUM_VRAM			(NUM_ROW + 1)	
#define STG_TIP_NUM	 		(NUM_ROW)	

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_NPW_col_0[5] = {"NO", "上电时间", "掉电时间"};
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void	NPW_Btn_hdl(void *self, uint8_t	btn_id);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int NPW_Entry(int row, int col, void *pp_text)
{
	char 						**pp = (char **)pp_text;
	Storage					*stg = Get_storage();
	rcd_alm_pwr_t		stg_alm;
	struct 		tm 		t;
	short						r = 0, pic_num = 0;
	if(col >2)
		return 0;
	
	r = row % STRIPE_MAX_ROWS;		//条纹界面上的行数是11
	pic_num = row / STRIPE_MAX_ROWS + 1;
	if(row > 0)
	{
		//(row - 1 * pic_num) 每页第0行显示的是标题栏，不能算入信息计数
		STG_Set_file_position(STG_LOSE_PWR, STG_DRC_READ, (row - 1 * pic_num)* sizeof(rcd_alm_pwr_t));
		if(stg->rd_stored_data(stg, STG_LOSE_PWR, \
				&stg_alm, sizeof(rcd_alm_pwr_t)) != sizeof(rcd_alm_pwr_t))
			{	
				//或者已经读完了
				STG_SELF.total_row = row - 1;
				return 0;
			}
			
		if(stg_alm.flag == 0xff)
		{
			STG_SELF.total_row = row - 1;
			return 0;		//记录结尾
		}
		STG_SELF.total_row = row + 1;
	}
	
	if(r == 0)
	{
		
		*pp = arr_NPW_col_0[col];
		return strlen(arr_NPW_col_0[col]);
	}
	
	
		
	switch(col)
	{
		case 0:
			sprintf(arr_p_vram[r], "%d", row - 1 * pic_num);
			break;
		case 1:
			if((stg_alm.happen_time_s == 0xffffffff) || (stg_alm.flag == 0xff))
			{
				sprintf(arr_p_vram[r], "                ");
				break;
			}
			Sec_2_tm(stg_alm.happen_time_s, &t);

			sprintf(arr_p_vram[r], "%2d%02d%02d-%02d:%02d:%02d", t.tm_year,t.tm_mon, t.tm_mday, \
					t.tm_hour, t.tm_min, t.tm_sec);
			break;
		case 2:
			if((stg_alm.disapper_time_s == 0xffffffff) || (stg_alm.flag == 0xff))
			{
				sprintf(arr_p_vram[r], "                ");
				break;
			}
			Sec_2_tm(stg_alm.disapper_time_s, &t);
			sprintf(arr_p_vram[r], "%2d%02d%02d-%02d:%02d:%02d", t.tm_year,t.tm_mon, t.tm_mday, \
					t.tm_hour, t.tm_min, t.tm_sec);
			break;
		default:
			return 0;
		
	}
	*pp = arr_p_vram[r];
	return strlen(arr_p_vram[r]);

}

static int NPW_Init(void *arg)
{
	int i = 0;
	
	
	HMI_Ram_init();
	for(i = 0; i < STG_NUM_VRAM; i++) {
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	
	
	STG_SELF.total_col = 3;
	STG_SELF.total_row = STRIPE_MAX_ROWS;		
	
	return RET_OK;
}

static void NPW_Build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	Progress_bar	*p_bar = PGB_Get_Sington();

	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(3, BTN_TYPE_ERASE, NPW_Btn_hdl, arg);
		
}
static void NPW_Exit(void)
{
	
}
static int NPW_Commit(void *arg)
{
	
	
	STG_Erase_file(STG_LOSE_PWR);
	STG_Set_alm_pwr_num(STG_LOSE_PWR, 0);
	LOG_Add(LOG_LOST_PWR_ERASE);
	phn_sys.pwr_rcd_index = 0xff;
//	g_news_power.cmd_hdl(g_news_power.p_cmd_rcv, sycmd_reflush, NULL);
	
	return RET_OK;
	
}

static int NPW_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf)
{

	

	return 0;
	
}

static int NPW_Key_UP(void *arg)
{
	
	
	
	return -1;
}
static int NPW_Key_DN(void *arg)
{
	
	return -1;
}
static int NPW_Key_LT(void *arg)
{
	
	return -1;
}

static int NPW_Key_RT(void *arg)
{
	
	return -1;
}
static int NPW_Key_ET(void *arg)
{
	return -1;
	
}




static void	NPW_Btn_hdl(void *self, uint8_t	btn_id)
{

	if(btn_id == ICO_ID_ERASETOOL)
	{
//		STG_Erase_file(STG_LOSE_PWR);
//		STG_Set_alm_pwr_num(STG_LOSE_PWR, 0);
//		LOG_Add(LOG_LOST_PWR_ERASE);
//		phn_sys.pwr_rcd_index = 0xff;
//		g_news_power.cmd_hdl(g_news_power.p_cmd_rcv, sycmd_reflush, NULL);
		
		
		sprintf(arr_p_vram[STG_TIP_NUM],"删除掉电信息？");
		Win_content(arr_p_vram[STG_TIP_NUM]);
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
		
		
		
		
	}
	
}




