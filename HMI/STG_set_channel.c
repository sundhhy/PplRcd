#include <stdint.h>
#include "HMI_striped_background.h"
#include "ModelFactory.h"
#include "utils/Storage.h"
#include "utils/log.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static int ChnStrategy_entry(int row, int col, void *pp_text);
static void CNS_build_component(void *arg);
static int Cns_key_up(void *arg);
static int Cns_key_dn(void *arg);
static int Cns_key_lt(void *arg);
static int Cns_key_rt(void *arg);
static int Cns_key_er(void *arg);
static int Cns_init(void *arg);
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
static int CNS_commit(void *arg);
static void CNS_Exit(void);

strategy_t	g_chn_strategy = {
	ChnStrategy_entry,
	Cns_init,
	CNS_build_component,
	Cns_key_up,
	Cns_key_dn,
	Cns_key_lt,
	Cns_key_rt,
	Cns_key_er,
	Cns_get_focusdata,
	CNS_commit,
	CNS_Exit,
	
};
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
//int phn_sys.key_weight = 1;			//该值会根据按键的动作而变化
//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
enum {
	row_chn_num,
	row_tag,
	row_signal_type,
	row_units,
	row_low_limit,
	row_upper_limit,
	row_MB,
	row_filter_time,
	row_small_signal,
	row_k,
	row_b,
	row_erase,
	row_num_rcd,
	row_num
}cns_rows;
#define STG_NUM_VRAM			(row_num + 1)
#define STG_RUN_VRAM_NUM		row_num
#define STG_SELF				g_chn_strategy
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	short		cur_page;
	short		cur_chn;
	char		win_buf[48];
	chn_info_t	tmp_info[NUM_CHANNEL];
	int			arr_flag_change[NUM_CHANNEL];

	
}cns_run_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static char *const arr_p_chnnel_entry[row_num] = {"通道号", "位号", "信号类型", "工程单位", \
 "量程下限", "量程上限", "记录容量", "滤波时间", "小信号切除", "零点调整 K", "零点调整 B", "清除数据", "记录数量"
};


	

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Cns_update_len(strategy_focus_t *p_syf);
static void Cns_update_content(int op, int weight);
static void CNS_Set_mdl_tmp_buf(int chn, chn_info_t *p_info);		
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int ChnStrategy_entry(int row, int col, void *pp_text)
{
	char 		**pp = (char **)pp_text;
	cns_run_t	*p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	Model_chn	*p_mc;
	Model		*p_md;
	Storage		*stg = Get_storage();
	
	
	p_mc = Get_Mode_chn(p_run->cur_chn);
	p_md = SUPER_PTR(p_mc, Model);
	
	
	
	if(row >= row_num)
			return 0;
	
	if(col == 0) {
		
		
		*pp = arr_p_chnnel_entry[row];
		return strlen(arr_p_chnnel_entry[row]);
	} else if(col == 1){
		
		if(row >= STRIPE_MAX_ROWS)
			p_run->cur_page = 1;
		else
			p_run->cur_page = 0;
		
		
		
		
		switch(row) 
		{
			case row_chn_num:
				sprintf(arr_p_vram[row], "%d", p_run->cur_chn);
				break;
			case row_tag:		//位号
				sprintf(arr_p_vram[row], "%d", p_mc->chni.tag_NO);
				break;
			case row_signal_type:		//信号类型
				p_md->to_string(p_md, AUX_SIGNALTYPE, arr_p_vram[row]);
				break;
			case row_units:		//单位
				p_md->to_string(p_md, AUX_UNIT, arr_p_vram[row]);
				
			
				break;
			case row_low_limit:		//下限
				p_md->to_string(p_md, chnaux_lower_limit, arr_p_vram[row]);
				break;
			case row_upper_limit:		//上限
				p_md->to_string(p_md, chnaux_upper_limit, arr_p_vram[row]);
				break;
			case row_MB:		//记录容量
				p_md->to_string(p_md, chnaux_record_mb, arr_p_vram[row]);
				break;
			case row_filter_time:		//滤波时间
				p_md->to_string(p_md, chnaux_filter_ts, arr_p_vram[row]);
				break;
			case row_small_signal:		//小信号切除
				p_md->to_string(p_md, chnaux_small_signal, arr_p_vram[row]);
				break;
			case row_k:		//零点调整
				p_md->to_string(p_md, chnaux_k, arr_p_vram[row]);
				
				break;
			case row_b:		//零点调整
				p_md->to_string(p_md, chnaux_b, arr_p_vram[row]);
				break;
			case row_erase:		//零点调整
				
				sprintf(arr_p_vram[row], "...");	
				break;
			case row_num_rcd:
				sprintf(arr_p_vram[row], "%-5d/%-5d", \
					stg->arr_rcd_mgr[p_run->cur_chn].rcd_count, stg->arr_rcd_mgr[p_run->cur_chn].rcd_maxcount);	
			
				break;
			default:
				goto exit;
			
		}
		
		//发生翻页的时候，要把焦点也进行切换
		//目前只考虑了2页的情况
		if(row == 0)
		{
			STG_SELF.sf.f_row = 0;
			Cns_update_len(&STG_SELF.sf);
		}
		else if(row == STRIPE_MAX_ROWS)
		{
			STG_SELF.sf.f_row = STRIPE_MAX_ROWS;
			Cns_update_len(&STG_SELF.sf);
		}
		
		*pp = arr_p_vram[row];
		return strlen(arr_p_vram[row]);
		
		
	}
	exit:	
	return 0;
}

static int Cns_init(void *arg)
{
	int i = 0;
	cns_run_t	*p_run;
	memset(&STG_SELF.sf, 0, sizeof(STG_SELF.sf));
	STG_SELF.sf.f_col = 1;
	STG_SELF.sf.f_row = 0;
	STG_SELF.sf.start_byte = 0;
	STG_SELF.sf.num_byte = 1;
	HMI_Ram_init();
	for(i = 0; i < STG_NUM_VRAM; i++) {
		
		
		
		if(i != STG_RUN_VRAM_NUM)
		{
			arr_p_vram[i] = HMI_Ram_alloc(48);
			memset(arr_p_vram[i], 0, 48);
		}
		else
		{
			arr_p_vram[i] = HMI_Ram_alloc(sizeof(cns_run_t));
			memset(arr_p_vram[i], 0, sizeof(cns_run_t));
		}
			
	}
	STG_SELF.total_col = 2;
	STG_SELF.total_row = row_num;
	p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	p_run->cur_page = 0;
	p_run->cur_chn = 0;
	
	for(i = 0; i < NUM_CHANNEL; i++)
		CNS_Set_mdl_tmp_buf(i, &p_run->tmp_info[i]);
	return RET_OK;
}


static void CNS_Exit(void)
{
	int i;
	for(i = 0; i < NUM_CHANNEL; i++)
		CNS_Set_mdl_tmp_buf(i, NULL);
}
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_syf->f_row >= row_num) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
	ret = p_syf->num_byte;
	
	
	
	*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	

	
	
	
	
	return ret;
	
}

static void CNS_Set_mdl_tmp_buf(int chn, chn_info_t *p_info)
{
	Model_chn			*p_mc = Get_Mode_chn(chn);
	
	MCH_Set_info_buf(p_mc, p_info, sizeof(chn_info_t));
	
	
}

static int Cns_key_up(void *arg)
{
	

	int 			ret = RET_OK;

	Cns_update_content(OP_ADD, phn_sys.key_weight);
	
	
	return ret;
}

static int Cns_key_dn(void *arg)
{

	int 				ret = RET_OK;
	
	
	Cns_update_content(OP_SUB, phn_sys.key_weight);

	return ret;
}


 static void	CNS_Btn_hdl(void *self, uint8_t	btn_id)
 {
	 short		i;
	 short 		flag = 0;
	 cns_run_t *p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	 if(btn_id == BTN_TYPE_SAVE)
	 {
		for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
		{
			if(p_run->arr_flag_change[i])
			{
				
				
				
				if(MdlChn_Commit_conf(i) == RET_OK)
				{
					
					flag = 1;
//					sprintf(p_run->win_buf,"写入配置成功");
//					Win_content(p_run->win_buf);
//					STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				else
				{
					flag = 2;
					break;
//					sprintf(p_run->win_buf,"通道[%d] 写入配置失败", i);
//					Win_content(p_run->win_buf);
//					STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				
			}
			p_run->arr_flag_change[i] = 0;
			
		}	
		
		
		if(flag == 1)
		{
			sprintf(p_run->win_buf,"写入配置成功");
			Win_content(p_run->win_buf);
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
			
		}
		else if(flag == 2)
		{
			
			sprintf(p_run->win_buf,"通道[%d] 写入配置失败", i);
			Win_content(p_run->win_buf);
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
		}

		 
	 }
	
	 
 }
static void CNS_build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(3, BTN_TYPE_SAVE, CNS_Btn_hdl, arg);
		
	
	
}

static int Cns_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	int ret = RET_OK;
	cns_run_t	*p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];

	if(p_run->cur_page == 0) {
		
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_ADD, 1, 0, STRIPE_MAX_ROWS - 1);
		if(p_syf->f_row == 0)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
		} 
		
	} else {
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_ADD, 1, STRIPE_MAX_ROWS, row_num - 1);
		
		if(p_syf->f_row == STRIPE_MAX_ROWS)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
		}
	}
	
	if((p_syf->f_row == row_tag) || (p_syf->f_row == row_MB))
		p_syf->f_row ++;
		
	Cns_update_len(p_syf);
	return ret;
}


static int Cns_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	int ret = RET_OK;
	cns_run_t	*p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	
	
	if(p_run->cur_page == 0)
	{
		
		if(p_syf->f_row == 0)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
			goto exit;
		}
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_SUB, 1, 0, STRIPE_MAX_ROWS - 1);
		
		
		
		
	}
	else
	{
		
		if(p_syf->f_row == STRIPE_MAX_ROWS)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
			goto exit;
		}
		p_syf->f_row = Operate_in_range(p_syf->f_row, OP_SUB, 1, STRIPE_MAX_ROWS, row_num - 1);
		
		
	
		
	}
	
	
	
	if((p_syf->f_row == row_tag) || (p_syf->f_row == row_MB))
		p_syf->f_row --;
	
	Cns_update_len(p_syf);
exit:

	return ret;
}



static int Cns_key_er(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	if(p_syf->f_row != row_erase)
	{
		return -1;
		
	}
	Win_content("删除通道数据?");
	STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, arr_p_vram[p_syf->f_row]);
	
	return RET_OK;
}

static int CNS_commit(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	cns_run_t	*p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	if(p_syf->f_row != row_erase)
	{
		return 0;
		
	}
	//删除通道数据
	LOG_Add(LOG_CHN_DATA_HANDLE_ERASE(p_run->cur_chn));
	STG_Erase_file(STG_CHN_DATA(p_run->cur_chn));
	return RET_OK;
}


static void Cns_update_len(strategy_focus_t *p_syf)
{
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	
	//把单位剔除掉
	switch(p_syf->f_row)
	{
		case 6:		//x M
		case 7:		//x S
		case 8:		//x %
			p_syf->num_byte -= 2;
			break;
		
	}
	
	
}

static void Cns_update_content(int op, int weight)
{
	strategy_focus_t 	*p_syf = &STG_SELF.sf;
	cns_run_t			*p_run = (cns_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	strategy_focus_t	pos;
	Model_chn			*p_mc;
	Model				*p_md;
	
	
	p_mc = Get_Mode_chn(p_run->cur_chn);
	p_md = SUPER_PTR(p_mc, Model);
	
	

	
	
	
	
	p_run->arr_flag_change[p_run->cur_chn] = 1;
	
	switch(p_syf->f_row) 
	{
		case row_chn_num:
			p_run->cur_chn = Operate_in_range(p_run->cur_chn, op, 1, 0, phn_sys.sys_conf.num_chn - 1);
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush, NULL);
//			Str_Calculations(arr_p_vram[p_syf->f_row], 1,  op, weight, 0, NUM_CHANNEL);
			break;
		case row_tag:		//位号
			p_run->arr_flag_change[p_run->cur_chn] = 0;
//			p_md->modify_str_conf(p_md, chnaux_tag_NO, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_signal_type:		//信号类型
			p_md->modify_str_conf(p_md, AUX_SIGNALTYPE, arr_p_vram[p_syf->f_row], op, weight);
			
			//要把上下限重新显示
			pos.f_col = 1;
			pos.f_row = 4;
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush_position, &pos);
		
			pos.f_col = 1;
			pos.f_row = 5;
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_reflush_position, &pos);
			break;
		case row_units:		//单位
			p_md->modify_str_conf(p_md, AUX_UNIT, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_low_limit:		//下限
			p_md->modify_str_conf(p_md, chnaux_lower_limit, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_upper_limit:		//上限
			p_md->modify_str_conf(p_md, chnaux_upper_limit, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_MB:		//记录容量
//			p_md->modify_str_conf(p_md, chnaux_record_mb, arr_p_vram[p_syf->f_row], op, weight);
			p_run->arr_flag_change[p_run->cur_chn] = 0;

			break;
		case row_filter_time:		//滤波时间
			p_md->modify_str_conf(p_md, chnaux_filter_ts, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_small_signal:		//小信号切除
			p_md->modify_str_conf(p_md, chnaux_small_signal, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_k:		//零点调整
			p_md->modify_str_conf(p_md, chnaux_k, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case row_b:		//零点调整
			p_md->modify_str_conf(p_md, chnaux_b, arr_p_vram[p_syf->f_row], op, weight);
			break;
		default:
			p_run->arr_flag_change[p_run->cur_chn] = 0;
			break;
		
	}
	
	Cns_update_len(p_syf);
	
	
}


