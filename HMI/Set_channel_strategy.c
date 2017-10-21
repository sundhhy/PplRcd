#include <stdint.h>
#include "Setting_HMI.h"

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
static int Cns_get_focusdata(void *pp_data);
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
	
 static char *const arr_p_chnnel_entry_2[2] = {"K:", "B:"};
 static char *arr_p_vram[14];

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
	if(col == 0) {
		
		if(row > 9)
			return 0;
		*pp = arr_p_chnnel_entry[row];
		return strlen(arr_p_chnnel_entry[row]);
	} else {
		if(col > 2)
			return 0;
		
		if(row == 9) {
			*pp = arr_p_chnnel_entry_2[col];
			return strlen(arr_p_chnnel_entry_2[row]);
		}
		
	}
	
	return 0;
}

static int Cns_init(void *arg)
{
	int i = 0;
	memset(&g_chn_strategy.sf, 0, sizeof(g_chn_strategy.sf));
	g_chn_strategy.sf.f_col = 1;
	g_chn_strategy.sf.f_row = 0;
	
	VRAM_init();
	for(i = 0; i < 10; i++) {
		
		arr_p_vram[i] = VRAM_alloc(48);
		
	}
	
	return RET_OK;
}
static int Cns_get_focusdata(void *pp_data)
{
	strategy_focus_t *p_syf = &g_chn_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = p_syf->num_byte;
	
	if(p_syf->f_row < 14)
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
		dl = Cns_get_focusdata(&p);
		if(dl < 0)
			return -1;
		
		
		switch(p_syf->f_row) {
			case 0:
				//时间参数修改
				
				Str_Calculations(p, dl,0, OP_ADD, 1, 0, 0);
				break;
			default:
				ret = -1;
				break;
			
		}
		
	} else if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		
		ret = -1;
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
		dl = Cns_get_focusdata(&p);
		if(dl < 0)
			return -1;
		
		
		switch(p_syf->f_row) {
			case 0:
				//时间参数修改
				
				Str_Calculations(p, dl,0, OP_SUB, 1, 0, 0);
				break;
			default:
				ret = -1;
				break;
			
		}
		
	} else if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		
		ret = -1;
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


