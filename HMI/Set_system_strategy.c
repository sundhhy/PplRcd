#include "Set_system_strategy.h"
#include "sdhDef.h"
#include "ModelFactory.h"
#include "system.h"
#include <string.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
static int SysStrategy_entry(int row, int col, void *pp_text);
static int Sys_key_up(void *arg);
static int Sys_key_dn(void *arg);
static int Sys_key_lt(void *arg);
static int Sys_key_rt(void *arg);
static int Sys_key_er(void *arg);
static int Sys_init(void *arg);
static int Sys_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf);
static int Sys_commit(void *arg);
static void SYS_Exit(void);

strategy_t	g_sys_strategy = {
	SysStrategy_entry,
	Sys_init,
	Sys_key_up,
	Sys_key_dn,
	Sys_key_lt,
	Sys_key_rt,
	Sys_key_er,
	Sys_get_focusdata,
	Sys_commit,
	SYS_Exit,

};
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

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 static char *const arr_p_sys_entry[14] = {"时间设定", "用户密码", "通道数目", "记录间隔", \
	"断偶处理", "断阻处理", "通信方式", "波特率", "本机地址", "调节参数修改", "冷端补偿", "通道状态显示", \
	 "按键声音", "恢复出厂设置"};
 
static int	sys_page = 0;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Sys_update_syf(strategy_focus_t *p_syf);
static int Sys_update_content(int op, int weight);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int SysStrategy_entry(int row, int col, void *pp_text)
{
	char 	**pp = (char **)pp_text;
	Model	*model;
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	if(col == 0) {
		if(row > 13)
			return 0;
		*pp = arr_p_sys_entry[row];
		return strlen(arr_p_sys_entry[row]);
	} else if(col == 1){
		switch(row) {
			case 0:
				
				sys_page = 0;
				p_syf->f_row = 0;
				Sys_update_syf(p_syf);
			
				model = ModelCreate("time");
				model->to_string(model, 1, arr_p_vram[0]);
				p_syf->num_byte = strlen(arr_p_vram[0]);
				break;
			case 1:
				System_to_string(NULL, arr_p_vram[row], 48, es_psd);
				break;
			case 2:
				sprintf(arr_p_vram[row], "%d", phn_sys.sys_conf.num_chn);
				break;
			case 3:
				sprintf(arr_p_vram[row], "%d", phn_sys.sys_conf.record_gap_s);
				break;
			case 4:
				System_to_string(NULL, arr_p_vram[row], 48, es_brk_cpl);
				break;
			case 5:
				System_to_string(NULL, arr_p_vram[row], 48, es_brk_rss);
				break;
			case 6:
				System_to_string(NULL, arr_p_vram[row], 48, es_cmn_md);
				break;
			case 7:
				sprintf(arr_p_vram[row], "%d", phn_sys.sys_conf.baud_rate);
				break;
			case 8:
				sprintf(arr_p_vram[row], "%d", phn_sys.sys_conf.id);
				break;
			case 9:
				System_to_string(NULL, arr_p_vram[row], 48, es_mdfy_prm);
				break;
			case 10:
				System_to_string(NULL, arr_p_vram[row], 48, es_CJC);
				break;
			case 11:
				
				System_to_string(NULL, arr_p_vram[row], 48, es_vcs);
				break;
			case 12:
				System_to_string(NULL, arr_p_vram[row], 48, es_beep);
				break;
			case 13:
				
				sprintf(arr_p_vram[row], "....");
			
				sys_page = 1;
				p_syf->f_row = 11;
				Sys_update_syf(p_syf);
				break;
			default:
				goto exit;
		}
		*pp = arr_p_vram[row];
		return strlen(*pp);
		
	}
	exit:
	return 0;
}

static int Sys_init(void *arg)
{
	int i = 0;
	memset(&g_sys_strategy.sf, 0, sizeof(g_sys_strategy.sf));
	g_sys_strategy.sf.f_col = 1;
	g_sys_strategy.sf.f_row = 0;
	g_sys_strategy.sf.num_byte = 1;
	
	VRAM_init();
	for(i = 0; i < 14; i++) {
		
		arr_p_vram[i] = VRAM_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	return RET_OK;
}
static void SYS_Exit(void)
{
	
}
static int Sys_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_in_syf)
		p_syf = p_in_syf;
	
	
	if(p_syf->f_row < 14) {
		*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
		p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
		ret = p_syf->num_byte;
	}
	else 
		ret = -1;
	
	
	return ret;
	
}



static int Sys_key_up(void *arg)
{
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int 			ret = RET_OK;
	
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		g_set_weight += 10;
		
	} else {
		g_set_weight = 1;
	}

	


	ret = Sys_update_content(OP_ADD, g_set_weight);
	return ret;
}

static int Sys_key_dn(void *arg)
{
	
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int 			ret = RET_OK;
	
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		g_set_weight += 10;
		
	} else {
		g_set_weight = 1;
	}

	


	ret = Sys_update_content(OP_SUB, g_set_weight);


	
	
	return ret;
}

static int Sys_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int ret = RET_OK;
//	switch(p_syf->f_row) {
//		case 0:
//			g_sys_strategy.sf.num_byte = 1;
//			if(p_syf->start_byte == 17)
//				p_syf->start_byte = 0;
//			else {
//				p_syf->start_byte += 1;
//			}
//			
//			break;
//		default:
//			ret = ERR_OPT_FAILED;
//			break;
//		
//		
//	}
//	ret = -1;
	if(sys_page == 0) {
		
		p_syf->f_row = Operate_in_tange(p_syf->f_row, OP_ADD, 1, 0, 10);
		
		if(p_syf->f_row == 0)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
		} else if(p_syf->f_row == 2)		{//通道数目不允许配置
		
			p_syf->f_row ++;
			
		}
		
	} else {
		p_syf->f_row = Operate_in_tange(p_syf->f_row, OP_ADD, 1, 11, 13);
		
		if(p_syf->f_row == 11)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
		}
	}
	
	
	
	Sys_update_syf(p_syf);
	return ret;
}

static int Sys_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int ret = RET_OK;
//	switch(p_syf->f_row) {
//		case 0:
//			g_sys_strategy.sf.num_byte = 1;
//			if(p_syf->start_byte == 0)
//				p_syf->start_byte = 17;
//			else {
//				p_syf->start_byte -= 1;
//			}
//			break;
//		default:
//			ret = ERR_OPT_FAILED;
//			break;
//		
//		
//	}
	
	if(sys_page == 0) {
		if(p_syf->f_row == 0)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
		}
		p_syf->f_row = Operate_in_tange(p_syf->f_row, OP_SUB, 1, 0, 10);
		
		if(p_syf->f_row == 2)		{//通道数目不允许配置
		
			p_syf->f_row --;
			
		}
		
	} else {
		if(p_syf->f_row == 11)
		{
			//说明光标发生了反转
			//只有超过范围才会反转
			ret = -1;
		}
		p_syf->f_row = Operate_in_tange(p_syf->f_row, OP_SUB, 1, 11, 13);
		
		
	}
	
	Sys_update_syf(p_syf);
	return ret;
}



static int Sys_key_er(void *arg)
{
	//将所有的配置项写入模型

	strategy_focus_t 	*p_syf = &g_sys_strategy.sf;
	int								ret = RET_OK;
	switch(p_syf->f_row) {
	case 0:	
		g_sys_strategy.cmd_hdl(g_sys_strategy.p_cmd_rcv, sycmd_win_time, arr_p_vram[0]);
		break;
	case 1:
		g_sys_strategy.cmd_hdl(g_sys_strategy.p_cmd_rcv, sycmd_win_psd, arr_p_vram[p_syf->f_row]);
		break;
	case 13:
		//恢复出厂设置，应该不只是恢复系统设置，包括通道设置等，应该也要恢复
		Win_content("确认恢复出厂设置？");
		g_sys_strategy.cmd_hdl(g_sys_strategy.p_cmd_rcv, sycmd_win_tips, arr_p_vram[p_syf->f_row]);
		
		break;
	default:
		ret = ERR_OPT_FAILED;
		break;
	
	
	}
	

	
	
	return ret;
}

static int Sys_commit(void *arg)
{
	//将所有的配置项写入模型

	strategy_focus_t 	*p_syf = &g_sys_strategy.sf;
	Model							*model;
	int								ret = RET_OK;
	
	
	
	switch(p_syf->f_row) {
	case 0:

		model = ModelCreate("time");
		ret = model->set_by_string(model, 1, arr_p_vram[0]);
			
		break;
	case 1:
		Password_set_by_str(arr_p_vram[p_syf->f_row]);
		break;
	case 13:
		System_default();
		break;
	default:
		ret = ERR_OPT_FAILED;
		break;
	
	
	
	}

	
	
	return ret;
}

static void Sys_update_syf(strategy_focus_t *p_syf)
{
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	
//	//把单位剔除掉
//	switch(p_syf->f_row)
//	{
//		case 6:		//x M
//		case 7:		//x S
//		case 8:		//x %
//			p_syf->num_byte -= 2;
//			break;
//		
//	}
//	
	
}


static int Sys_update_content(int op, int weight)
{
	strategy_focus_t 	*p_syf = &g_sys_strategy.sf;
	int					ret = RET_OK;
	switch(p_syf->f_row) {
	case 0:		
		g_sys_strategy.cmd_hdl(g_sys_strategy.p_cmd_rcv, sycmd_win_time, arr_p_vram[0]);
		ret = 1;
		break;
	case 1:
		g_sys_strategy.cmd_hdl(g_sys_strategy.p_cmd_rcv, sycmd_win_psd, arr_p_vram[p_syf->f_row]);
		ret = 1;
		break;
	case 2:		//通道数目不允许配置
		break;
	case 3:
		System_modify_string(arr_p_vram[p_syf->f_row], es_rcd_t_s, op, weight);
		break;
	case 4:
		System_modify_string(arr_p_vram[p_syf->f_row], es_brk_cpl, op, weight);
		break;
	case 5:
		System_modify_string(arr_p_vram[p_syf->f_row], es_brk_rss, op, weight);
		break;
	case 6:
		System_modify_string(arr_p_vram[p_syf->f_row], es_cmn_md, op, weight);
		break;
	case 7:
		System_modify_string(arr_p_vram[p_syf->f_row], es_baud, op, weight);
		break;
	case 8:
		System_modify_string(arr_p_vram[p_syf->f_row], es_id, op, weight);
		break;
	case 9:
		System_modify_string(arr_p_vram[p_syf->f_row], es_mdfy_prm, op, weight);
		break;
	case 10:
		System_modify_string(arr_p_vram[p_syf->f_row], es_CJC, op, weight);
		break;
	case 11:
		System_modify_string(arr_p_vram[p_syf->f_row], es_vcs, op, weight);
		break;
	case 12:
		System_modify_string(arr_p_vram[p_syf->f_row], es_beep, op, weight);
		break;
	default:
		break;
	
	
	}
	return ret;
}

