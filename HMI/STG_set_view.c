#include <stdint.h>
#include "HMI_striped_background.h"

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static int ViewStrategy_entry(int row, int col, void *pp_text);
static int VIEW_init(void *arg);
static int VIEW_key_up(void *arg);
static int VIEW_key_dn(void *arg);
static int VIEW_key_lt(void *arg);
static int VIEW_key_rt(void *arg);
static int VIEW_key_er(void *arg);
static int VIEW_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf);
static int VIEW_commit(void *arg);
static void VIEW_Exit(void);

strategy_t	g_view_strategy = {
	ViewStrategy_entry,
	VIEW_init,
	STY_Duild_button,
	VIEW_key_up,
	VIEW_key_dn,
	VIEW_key_lt,
	VIEW_key_rt,
	VIEW_key_er,
	VIEW_get_focusdata,
	VIEW_commit,
	VIEW_Exit,
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
#define STG_SELF  g_view_strategy
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_p_view_entry[7] = {"曲线组：", "曲线1：", "曲线2：", "曲线3：", "曲线4：", "曲线5：",  "曲线6："
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

static int ViewStrategy_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	if(col == 0) {
		
		if(row > 6)
			return 0;
		*pp = arr_p_view_entry[row];
		return strlen(arr_p_view_entry[row]);
	} 
	
	return 0;
}

static int VIEW_init(void *arg)
{
	STG_SELF.total_col = 1;
	STG_SELF.total_row = 7;
	return RET_OK;
}
static void VIEW_Exit(void)
{
	
}
static int VIEW_key_up(void *arg)
{
	return RET_OK;
}
static int VIEW_key_dn(void *arg)
{
	return RET_OK;
}
static int VIEW_key_lt(void *arg)
{
	return RET_OK;
}
static int VIEW_key_rt(void *arg)
{
	return RET_OK;
}
static int VIEW_key_er(void *arg)
{
	return RET_OK;
}

static int VIEW_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf)
{
	return RET_OK;
}
static int VIEW_commit(void *arg)
{
	return RET_OK;
}


