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

static int Data_bacnup_Strategy_entry(int row, int col, void *pp_text);
static int DBP_init(void *arg);
static int DBP_key_up(void *arg);
static int DBP_key_dn(void *arg);
static int DBP_key_lt(void *arg);
static int DBP_key_rt(void *arg);
static int DBP_key_er(void *arg);
static int DBP_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf);
static int DBP_commit(void *arg);

strategy_t	g_DBU_strategy = {
	Data_bacnup_Strategy_entry,
	DBP_init,
	DBP_key_up,
	DBP_key_dn,
	DBP_key_lt,
	DBP_key_rt,
	DBP_key_er,
	DBP_get_focusdata,
	DBP_commit,
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
 static char *const arr_p_DBU_entry[6] = {"设备当前状态：", "备份数据通道：", "起始时间：", "终止时间：",\
	 "文件名：", "备份进程"
 };
	
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

static int Data_bacnup_Strategy_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	if(col == 0) {
		
		if(row > 5)
			return 0;
		*pp = arr_p_DBU_entry[row];
		return strlen(arr_p_DBU_entry[row]);
	} 
	
	return 0;
}

static int DBP_init(void *arg)
{
	return RET_OK;
}
static int DBP_key_up(void *arg)
{
	return RET_OK;
}
static int DBP_key_dn(void *arg)
{
	return RET_OK;
}
static int DBP_key_lt(void *arg)
{
	return RET_OK;
}
static int DBP_key_rt(void *arg)
{
	return RET_OK;
}
static int DBP_key_er(void *arg)
{
	return RET_OK;
}

static int DBP_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf)
{
	return RET_OK;
}
static int DBP_commit(void *arg)
{
	return RET_OK;
}


