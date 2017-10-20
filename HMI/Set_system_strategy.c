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

strategy_t	g_sys_strategy = {
	SysStrategy_entry,
	Sys_init,
	Sys_key_up,
	Sys_key_dn,
	Sys_key_lt,
	Sys_key_rt,
	Sys_key_er,
	0,
	0
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
				*pp = model->to_string(model, 1, arr_p_hmi_buf[0]);
				return strlen(*pp);
			
			default: break;
		}
		
	}
	
	return 0;
}

static int Sys_init(void *arg)
{
	memset(&g_sys_strategy.sf, 0, sizeof(g_sys_strategy.sf));
	g_sys_strategy.sf.f_col = 1;
	
	return -1;
}

static int Sys_key_up(void *arg)
{
	
	return -1;
}

static int Sys_key_dn(void *arg)
{
	
	return -1;
}

static int Sys_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int ret = RET_OK;
	switch(p_syf->f_row) {
		case 0:
			g_sys_strategy.sf.num_byte = 2;
			if(p_syf->start_byte == 6)
				p_syf->start_byte += 4;
			else if(p_syf->start_byte == 16)
				p_syf->start_byte = 0;
			else 
				p_syf->start_byte += 3;
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
			g_sys_strategy.sf.num_byte = 2;
			if(p_syf->start_byte == 0)
				p_syf->start_byte = 16;
			else if(p_syf->start_byte == 10)
				p_syf->start_byte -= 4;
			else 
				p_syf->start_byte -= 3;
			break;
		default:
			ret = ERR_OPT_FAILED;
			break;
		
		
	}
	
	return ret;
}



static int Sys_key_er(void *arg)
{
	
	return -1;
}