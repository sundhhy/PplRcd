#include "Set_system_strategy.h"
#include "sdhDef.h"
#include "ModelFactory.h"

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
static int Sys_get_focusdata(void *pp_data);

strategy_t	g_sys_strategy = {
	SysStrategy_entry,
	Sys_init,
	Sys_key_up,
	Sys_key_dn,
	Sys_key_lt,
	Sys_key_rt,
	Sys_key_er,
	Sys_get_focusdata,

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

static int SysStrategy_entry(int row, int col, void *pp_text)
{
	char 	**pp = (char **)pp_text;
	Model	*model;
	if(col == 0) {
		if(row > 13)
			return 0;
		*pp = arr_p_sys_entry[row];
		return strlen(arr_p_sys_entry[row]);
	} else {
		switch(row) {
			case 0:
				model = ModelCreate("time");
				*pp = model->to_string(model, 1, arr_p_vram[0]);
				return strlen(*pp);
			
			default: break;
		}
		
	}
	
	return 0;
}

static int Sys_init(void *arg)
{
	int i = 0;
	memset(&g_sys_strategy.sf, 0, sizeof(g_sys_strategy.sf));
	g_sys_strategy.sf.f_col = 1;
	g_sys_strategy.sf.f_row = 0;
	
	VRAM_init();
	for(i = 0; i < 14; i++) {
		
		arr_p_vram[i] = VRAM_alloc(48);
		
	}
	
	return RET_OK;
}
static int Sys_get_focusdata(void *pp_data)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = g_sys_strategy.sf.num_byte;
	
	if(p_syf->f_row < 14)
		*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	else 
		ret = -1;
	
	
	return ret;
	
}



static int Sys_key_up(void *arg)
{
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	char			*p;
	int				dl;
	int 			ret = RET_OK;
//	uint8_t		rl, rh;
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	
	
	
	
	if(kt.key_type == SY_KEYTYPE_HIT) {
		dl = Sys_get_focusdata(&p);
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

static int Sys_key_dn(void *arg)
{
	
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	char			*p;
	int				dl;
	int 			ret = RET_OK;
//	uint8_t		rl, rh;
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	
	
	
	
	if(kt.key_type == SY_KEYTYPE_HIT) {
		dl = Sys_get_focusdata(&p);
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

static int Sys_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int ret = RET_OK;
	switch(p_syf->f_row) {
		case 0:
			g_sys_strategy.sf.num_byte = 1;
			if(p_syf->start_byte == 17)
				p_syf->start_byte = 0;
			else {
				p_syf->start_byte += 1;
//				if(arr_p_vram[0][p_syf->start_byte] > '9' || arr_p_vram[0][p_syf->start_byte] < '0') {
//					
//					p_syf->start_byte ++;
//				}
			}
			
			break;
		default:
			ret = ERR_OPT_FAILED;
			break;
		
		
	}
	
	return ret;
}

static int Sys_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int ret = RET_OK;
	switch(p_syf->f_row) {
		case 0:
			g_sys_strategy.sf.num_byte = 1;
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



static int Sys_key_er(void *arg)
{
	//将所有的配置项写入模型

	
	Model	*model;
	
	
	
	model = ModelCreate("time");
	model->set_by_string(model, 1, arr_p_vram[0]);
	
	
	return RET_OK;
}


