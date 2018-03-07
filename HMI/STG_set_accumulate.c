#include <stdint.h>
#include "HMI_striped_background.h"
//#include "utils/Storage.h"
#include "channel_accumulated.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define ST_SELF			 g_set_ACC
#define SSA_CLEAR_NUM	NUM_CHANNEL
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static int SSA_Entry(int row, int col, void *pp_text);
static int SSA_Init(void *arg);
static void SSA_Build_component(void *arg);
static int SSA_Key_UP(void *arg);
static int SSA_Key_DN(void *arg);
static int SSA_Key_LT(void *arg);
static int SSA_Key_RT(void *arg);
static int SSA_Key_ET(void *arg);
static int SSA_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf);
static int SSA_Commit(void *arg);
static void SSA_Exit(void);

strategy_t	ST_SELF = {
	SSA_Entry,
	SSA_Init,
	SSA_Build_component,
	SSA_Key_UP,
	SSA_Key_DN,
	SSA_Key_LT,
	SSA_Key_RT,
	SSA_Key_ET,
	SSA_Get_focus_data,	
	SSA_Commit,
	SSA_Exit,
};


//static int Cns_init(void *arg);
//static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
//strategy_t	ST_SELF = {
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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
enum {
	row_chn_num,
	row_sum_enable,
	row_start_year,
	row_start_month,
	row_start_day,
	row_clear,
	row_max,
	
}SSA_rows;

#define SSA_NUM_RAM							(row_max	+ 2)
#define SSA_TIPS_RAM_NUM				row_max	
#define SSA_TEMP_RAM_NUM				row_max	+ 1		//前6个字节用于保存对应通道的累积配置是否被修改
//，第7个字节用于表示是否是清除数据
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_SSA_col_0[row_max] = {"通道号", "通道累积", "起始年份", "起始月份", "起始日", "累积清除"};
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void SSA_update_content(int op, int weight);
static void	SSA_Btn_hdl(void *self, uint8_t	btn_id);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int SSA_Entry(int row, int col, void *pp_text)
{
	char 						**pp = (char **)pp_text;
	if(col > 1)
		return 0;
	
	if(row >= row_max)
		return 0;
	
	if(col == 0)
	{
		*pp = arr_SSA_col_0[row];
		return strlen(arr_SSA_col_0[row]);
		
	}
	
	switch(row)
	{
		case row_chn_num:
			sprintf(arr_p_vram[row], "%d", g_setting_chn);
			break;
		case row_sum_enable:
			CNA_Print_enable(arr_p_vram[row], arr_chn_acc[g_setting_chn].enable_sum);
			break;
		case row_start_year:
			
			sprintf(arr_p_vram[row], "%2d", arr_chn_acc[g_setting_chn].sum_start_year);
			break;
		case row_start_month:
			sprintf(arr_p_vram[row], "%2d", arr_chn_acc[g_setting_chn].sum_start_month);
			break;		
		case row_start_day:
			sprintf(arr_p_vram[row], "%2d", arr_chn_acc[g_setting_chn].sum_start_day);
			break;		
		case row_clear:
			sprintf(arr_p_vram[row], "确定");
			break;
	}
		

	*pp = arr_p_vram[row];
	return strlen(arr_p_vram[row]);

}

static int SSA_Init(void *arg)
{
	int i = 0;
	
	memset(&ST_SELF.sf, 0, sizeof(ST_SELF.sf));
	ST_SELF.sf.f_col = 1;
	ST_SELF.sf.f_row = 0;
	ST_SELF.sf.start_byte = 0;
	ST_SELF.sf.num_byte = 1;
	g_setting_chn = 0;
	
	HMI_Ram_init();
	for(i = 0; i < SSA_NUM_RAM; i++) {
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	
	return RET_OK;
}

static void SSA_Build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();

	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(3, BTN_TYPE_SAVE, SSA_Btn_hdl, arg);
		
}


static void	SSA_Btn_hdl(void *self, uint8_t	btn_id)
{
	 int		i;
	 if(btn_id == BTN_TYPE_SAVE)
	 {
		for(i = 0; i < NUM_CHANNEL; i++)
		{
			if(arr_p_vram[SSA_TEMP_RAM_NUM][i])
			{
				if(CNA_Commit(i) == RET_OK)
				{
					arr_p_vram[SSA_TEMP_RAM_NUM][i] = 0;
					sprintf(arr_p_vram[SSA_TIPS_RAM_NUM],"写入配置成功");
					Win_content(arr_p_vram[SSA_TIPS_RAM_NUM]);
					ST_SELF.cmd_hdl(ST_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				else
				{
					
					sprintf(arr_p_vram[SSA_TIPS_RAM_NUM],"通道[%d] 写入配置失败", i);
					Win_content(arr_p_vram[SSA_TIPS_RAM_NUM]);
					ST_SELF.cmd_hdl(ST_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				
			}
			arr_p_vram[SSA_TEMP_RAM_NUM][i] = 0;
			
		}	

		 
	 }
	
	 
 }
static void SSA_Exit(void)
{
	
}


static int SSA_Get_focus_data(void *pp_data, strategy_focus_t *p_in_syf)
{

	strategy_focus_t *p_syf = &ST_SELF.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_syf->f_row >= row_max) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
	
	
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	ret = p_syf->num_byte;
	*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	return ret;
}

static int SSA_Key_UP(void *arg)
{
	SSA_update_content(OP_ADD, phn_sys.key_weight);
	return RET_OK;
}
static int SSA_Key_DN(void *arg)
{
	SSA_update_content(OP_SUB, phn_sys.key_weight);
	return RET_OK;
}
static int SSA_Key_RT(void *arg)
{
	strategy_focus_t *p_syf = &ST_SELF.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row < (row_max - 1))
	{
		p_syf->f_row ++;
	}
	else {
		p_syf->f_row = 0;
		p_syf->f_col = 1;
		ret = -1;
	}
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	return ret;
}

static int SSA_Key_LT(void *arg)
{
	
	strategy_focus_t *p_syf = &ST_SELF.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row )
	{
		p_syf->f_row --;
	}
	else {
		p_syf->f_row = row_max - 1;
		ret = -1;
	}
	
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	return ret;
}
static int SSA_Key_ET(void *arg)
{
	strategy_focus_t *p_syf = &ST_SELF.sf;
	
	if(p_syf->f_row == row_clear)
	{
		Win_content("确认删除累积值？");
		ST_SELF.cmd_hdl(ST_SELF.p_cmd_rcv, sycmd_win_tips, arr_p_vram[SSA_TIPS_RAM_NUM]);
		arr_p_vram[SSA_TEMP_RAM_NUM][SSA_CLEAR_NUM] = 1;

		return RET_OK;
	}
	return RET_FAILED;
}

static int SSA_Commit(void *arg)
{
//	strategy_focus_t *p_syf = &ST_SELF.sf;
	
	
	
	if(arr_p_vram[SSA_TEMP_RAM_NUM][SSA_CLEAR_NUM])
	{
		
		CNA_Clear(g_setting_chn);
		
		arr_p_vram[SSA_TEMP_RAM_NUM][SSA_CLEAR_NUM] = 0;

	}
	else
	{
		CNA_Commit(g_setting_chn);
		
	}
	
	
	
//	exit:
	return RET_OK;
	
}

static void SSA_update_content(int op, int weight)
{
	rcd_chn_accumlated_t		*p_acc = arr_chn_acc + g_setting_chn;
	strategy_focus_t 				*p_syf = &ST_SELF.sf;
	

	arr_p_vram[SSA_TEMP_RAM_NUM][g_setting_chn] = 1;		//记录修改
	switch(p_syf->f_row) 
	{
		case row_chn_num:
			g_setting_chn = Operate_in_tange(g_setting_chn, op, 1, 0, NUM_CHANNEL - 1);
			ST_SELF.cmd_hdl(ST_SELF.p_cmd_rcv, sycmd_reflush, NULL);
			arr_p_vram[SSA_TEMP_RAM_NUM][g_setting_chn] = 0;
			break;
		case row_sum_enable:	
			p_acc->enable_sum = Operate_in_tange(p_acc->enable_sum, op, 1, 0, 1);
			CNA_Print_enable(arr_p_vram[p_syf->f_row], p_acc->enable_sum);
			break;
		case row_start_year:	
			p_acc->sum_start_year = Operate_in_tange(p_acc->sum_start_year, op, weight, 0, 99);
			sprintf(arr_p_vram[p_syf->f_row], "%2d", p_acc->sum_start_year);
			break;
		case row_start_month:	
			p_acc->sum_start_month = Operate_in_tange(p_acc->sum_start_month, op, 1, 1, 12);
			sprintf(arr_p_vram[p_syf->f_row], "%2d", p_acc->sum_start_month);
			break;
		case row_start_day:	
			p_acc->sum_start_day = Operate_in_tange(p_acc->sum_start_day, op, 1, 1, 31);
			sprintf(arr_p_vram[p_syf->f_row], "%2d", p_acc->sum_start_day);
			break;
		default:
			arr_p_vram[SSA_TEMP_RAM_NUM][g_setting_chn] = 0;
			break;
		
	}
	
	
}



