#include <stdint.h>
#include "Setting_HMI.h"
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
static int Cns_key_up(void *arg);
static int Cns_key_dn(void *arg);
static int Cns_key_lt(void *arg);
static int Cns_key_rt(void *arg);
static int Cns_key_er(void *arg);
static int Cns_init(void *arg);
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
strategy_t	g_chn_strategy = {
	ChnStrategy_entry,
	Cns_init,
	Cns_key_up,
	Cns_key_dn,
	Cns_key_lt,
	Cns_key_rt,
	Cns_key_er,
	Cns_get_focusdata,
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
static char *const arr_p_chnnel_entry[10] = {"通道号", "位号", "信号类型", "工程单位", \
 "量程下限", "量程上限", "记录容量", "滤波时间", "小信号切除", "零点调整"
};

static char *arr_p_vram[11];
static char		cur_set_chn = 0;

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

static int ChnStrategy_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	Model_chn		*p_mc = Get_Mode_chn(cur_set_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
	if(col == 0) {
		
		if(row > 9)
			return 0;
		*pp = arr_p_chnnel_entry[row];
		return strlen(arr_p_chnnel_entry[row]);
	} else if(col == 1){
		switch(row) 
		{
			case 0:
				sprintf(arr_p_vram[row], "%d", cur_set_chn);
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
				p_md->to_string(p_md, AUX_CHN_lower_limit, arr_p_vram[row]);
				break;
			case 5:		//上限
				p_md->to_string(p_md, AUX_CHN_upper_limit, arr_p_vram[row]);
				break;
			case 6:		//记录容量
				sprintf(arr_p_vram[row], "%d M", p_mc->chni.MB);
				break;
			case 7:		//滤波时间
				sprintf(arr_p_vram[row], "%d S", p_mc->chni.filter_time_s);
				break;
			case 8:		//小信号切除
				p_md->to_string(p_md, AUX_CHN_small_signal, arr_p_vram[row]);
				break;
			case 9:		//零点调整
				p_md->to_string(p_md, AUX_CHN_K, arr_p_vram[row]);
				
				break;
			default:
				goto exit;
			
		}
		
		*pp = arr_p_vram[row];
		return strlen(arr_p_vram[row]);
		
		
	} else if(col == 2) {
		
		if(row == 9) {
			p_md->to_string(p_md, AUX_CHN_B, arr_p_vram[10]);
			*pp = arr_p_vram[10];
			return strlen(arr_p_vram[10]);
		}
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
	g_chn_strategy.sf.num_byte = 1;

	VRAM_init();
	for(i = 0; i < 11; i++) {
		
		arr_p_vram[i] = VRAM_alloc(48);
		
	}
	
	return RET_OK;
}
static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_in_syf)
		p_syf = p_in_syf;
	ret = p_syf->num_byte;
	if(p_syf->f_row < 11)
		*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	else 
		ret = -1;
	
	
	return ret;
	
}



static int Cns_key_up(void *arg)
{
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	char			*p;
	int				dl;
	int 			ret = RET_OK;
//	uint8_t		rl, rh;
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	
	
	
	
	if(kt.key_type == SY_KEYTYPE_HIT) {
		dl = Cns_get_focusdata(&p, NULL);
		if(dl < 0)
			return -1;
		
		
		switch(p_syf->f_row) {
			case 0:
				//时间参数修改
				
				Str_Calculations(p, dl,0, OP_ADD, 1, 0, NUM_CHANNEL);
				cur_set_chn = atoi(p);
				g_chn_strategy.cmd_hdl( g_chn_strategy.p_cmd_rcv, sycmd_reflush, NULL);
				break;
			default:
				ret = -1;
				break;
			
		}
		
	} else if(kt.key_type == SY_KEYTYPE_DHIT) {
		
		if(p_syf->f_row )
			p_syf->f_row --;
		else {
			p_syf->f_row = 9;
			ret = -1;
		}
	}
	
	return ret;
}

static int Cns_key_dn(void *arg)
{
	
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	char			*p;
	int				dl;
	int 			ret = RET_OK;
//	uint8_t		rl, rh;
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	
	
	
	
	if(kt.key_type == SY_KEYTYPE_HIT) {
		dl = Cns_get_focusdata(&p, NULL);
		if(dl < 0)
			return -1;
		
		
		switch(p_syf->f_row) {
			case 0:
				//时间参数修改
				Str_Calculations(p, dl,0, OP_SUB, 1, 0, NUM_CHANNEL);
				cur_set_chn = atoi(p);
				g_chn_strategy.cmd_hdl(g_chn_strategy.p_cmd_rcv, sycmd_reflush, NULL);
				break;
			default:
				ret = -1;
				break;
			
		}
		
	} else if(kt.key_type == SY_KEYTYPE_DHIT) {
		if(p_syf->f_row < 9)
			p_syf->f_row ++;
		else {
			p_syf->f_row = 0;
			ret = -1;
		}
	}
	
	return ret;
}

static int Cns_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	int ret = RET_OK;
	switch(p_syf->f_row) {
		case 0:
			p_syf->num_byte = 1;
			if(p_syf->start_byte == 17)
				p_syf->start_byte = 0;
			else {
				p_syf->start_byte += 1;

			}
			
			break;
		default:
			ret = ERR_OPT_FAILED;
			break;
		
		
	}
	
	return ret;
}

static int Cns_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	int ret = RET_OK;
	switch(p_syf->f_row) {
		case 0:
			p_syf->num_byte = 1;
			if(p_syf->start_byte == 0)
				p_syf->start_byte = 17;
			else {
				p_syf->start_byte -= 1;
			}
			break;
		default:
			ret = ERR_OPT_FAILED;
			break;
		
		
	}
	
	return ret;
}



static int Cns_key_er(void *arg)
{
	
	return -1;
}


