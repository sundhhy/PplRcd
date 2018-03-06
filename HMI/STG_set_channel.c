#include <stdint.h>
#include "HMI_striped_background.h"
#include "ModelFactory.h"

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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static char *const arr_p_chnnel_entry[11] = {"通道号", "位号", "信号类型", "工程单位", \
 "量程下限", "量程上限", "记录容量", "滤波时间", "小信号切除", "零点调整 K", "零点调整 B"
};


	

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Cns_update_len(strategy_focus_t *p_syf);
static void Cns_update_content(int op, int weight);
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
	char **pp = (char **)pp_text;
	Model_chn		*p_mc = Get_Mode_chn(g_setting_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
	if(col == 0) {
		
		if(row > 10)
			return 0;
		*pp = arr_p_chnnel_entry[row];
		return strlen(arr_p_chnnel_entry[row]);
	} else if(col == 1){
		switch(row) 
		{
			case 0:
				sprintf(arr_p_vram[row], "%d", g_setting_chn);
				break;
			case 1:		//位号
				sprintf(arr_p_vram[row], "%d", p_mc->chni.tag_NO);
				break;
			case 2:		//信号类型
				p_md->to_string(p_md, AUX_SIGNALTYPE, arr_p_vram[row]);
				break;
			case 3:		//单位
				p_md->to_string(p_md, AUX_UNIT, arr_p_vram[row]);
				
			
				break;
			case 4:		//下限
				p_md->to_string(p_md, chnaux_lower_limit, arr_p_vram[row]);
				break;
			case 5:		//上限
				p_md->to_string(p_md, chnaux_upper_limit, arr_p_vram[row]);
				break;
			case 6:		//记录容量
				p_md->to_string(p_md, chnaux_record_mb, arr_p_vram[row]);
				break;
			case 7:		//滤波时间
				p_md->to_string(p_md, chnaux_filter_ts, arr_p_vram[row]);
				break;
			case 8:		//小信号切除
				p_md->to_string(p_md, chnaux_small_signal, arr_p_vram[row]);
				break;
			case 9:		//零点调整
				p_md->to_string(p_md, chnaux_k, arr_p_vram[row]);
				
				break;
			case 10:		//零点调整
				p_md->to_string(p_md, chnaux_b, arr_p_vram[row]);
				
				break;
			default:
				goto exit;
			
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
	memset(&g_chn_strategy.sf, 0, sizeof(g_chn_strategy.sf));
	g_chn_strategy.sf.f_col = 1;
	g_chn_strategy.sf.f_row = 0;
	g_chn_strategy.sf.start_byte = 0;
	g_chn_strategy.sf.num_byte = 1;
	g_setting_chn = 0;
	HMI_Ram_init();
	for(i = 0; i < 12; i++) {
		
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	
//	phn_sys.key_weight = 1;
	return RET_OK;
}


static void CNS_Exit(void)
{
	
}
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_syf->f_row > 10) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
	ret = p_syf->num_byte;
	
	
	
	*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	

	
	
	
	
	return ret;
	
}



static int Cns_key_up(void *arg)
{
	
//	Model_chn			*p_mc = Get_Mode_chn(g_setting_chn);
//	Model				*p_md = SUPER_PTR(p_mc, Model);
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t 	*p_syf = &g_chn_strategy.sf;
//	char			*p;
	int 			ret = RET_OK;
	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
	
	//
//	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
//		phn_sys.key_weight += 10;
//		
//	} else {
//		phn_sys.key_weight = 1;
//	}
	Cns_update_content(OP_ADD, phn_sys.key_weight);
	
	
	return ret;
}

static int Cns_key_dn(void *arg)
{
	

//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	int 				ret = RET_OK;
	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
	

//	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
//		phn_sys.key_weight += 10;
//		
//	} else {
//		phn_sys.key_weight = 1;
//	}
	
	Cns_update_content(OP_SUB, phn_sys.key_weight);
	
		
	return ret;
}


 static void	CNS_Btn_hdl(void *self, uint8_t	btn_id)
 {
	 int		i;
	 if(btn_id == BTN_TYPE_SAVE)
	 {
		for(i = 0; i < NUM_CHANNEL; i++)
		{
			if(phn_sys.save_chg_flga & CHG_MODCHN_CONF(i))
			{
				
				
				
				if(MdlChn_Commit_conf(i) == RET_OK)
				{
					phn_sys.save_chg_flga &=~ CHG_MODCHN_CONF(i);
					sprintf(arr_p_vram[11],"写入配置成功");
					Win_content(arr_p_vram[11]);
					g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				else
				{
					
					sprintf(arr_p_vram[11],"通道[%d] 写入配置失败", i);
					Win_content(arr_p_vram[11]);
					g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				
			}
			
			
		}	

		 
	 }
	
	 
 }
static void CNS_build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(1, BTN_TYPE_SAVE, CNS_Btn_hdl, arg);
		
	
	
}

static int Cns_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row < 10)
		p_syf->f_row ++;
	else {
		p_syf->f_row = 0;
		p_syf->f_col = 1;
		ret = -1;
	}
		
	Cns_update_len(p_syf);
	return ret;
}


static int Cns_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row )
		p_syf->f_row --;
	else {
		p_syf->f_row = 10;
		ret = -1;
		
	}
	
	Cns_update_len(p_syf);
	return ret;
}



static int Cns_key_er(void *arg)
{
	
	return -1;
}

static int CNS_commit(void *arg)
{
	return 0;
	
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
	Model_chn			*p_mc = Get_Mode_chn(g_setting_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
	strategy_focus_t 	*p_syf = &g_chn_strategy.sf;
	
	strategy_focus_t		pos;

	
	
	
	
	
	
	switch(p_syf->f_row) 
	{
		case 0:
			g_setting_chn = Operate_in_tange(g_setting_chn, op, 1, 0, NUM_CHANNEL - 1);
			g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_reflush, NULL);
//			Str_Calculations(arr_p_vram[p_syf->f_row], 1,  op, weight, 0, NUM_CHANNEL);
			break;
		case 1:		//位号
//			Str_Calculations(arr_p_vram[p_syf->f_row], 1,  op, weight, 0, 9);
			p_md->modify_str_conf(p_md, chnaux_tag_NO, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 2:		//信号类型
			p_md->modify_str_conf(p_md, AUX_SIGNALTYPE, arr_p_vram[p_syf->f_row], op, weight);
			
			//要把上下限重新显示
			pos.f_col = 1;
			pos.f_row = 4;
			g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_reflush_position, &pos);
		
			pos.f_col = 1;
			pos.f_row = 5;
			g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_reflush_position, &pos);
			break;
		case 3:		//单位
			p_md->modify_str_conf(p_md, AUX_UNIT, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 4:		//下限
			p_md->modify_str_conf(p_md, chnaux_lower_limit, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 5:		//上限
			p_md->modify_str_conf(p_md, chnaux_upper_limit, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 6:		//记录容量
			p_md->modify_str_conf(p_md, chnaux_record_mb, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 7:		//滤波时间
			p_md->modify_str_conf(p_md, chnaux_filter_ts, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 8:		//小信号切除
			p_md->modify_str_conf(p_md, chnaux_small_signal, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 9:		//零点调整
			p_md->modify_str_conf(p_md, chnaux_k, arr_p_vram[p_syf->f_row], op, weight);
			break;
		case 10:		//零点调整
			p_md->modify_str_conf(p_md, chnaux_b, arr_p_vram[p_syf->f_row], op, weight);
			break;
		default:
			break;
		
	}
	
	Cns_update_len(p_syf);
	
	
}


