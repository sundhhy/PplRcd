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

static int ArtStrategy_entry(int row, int col, void *pp_text);
static int ART_init(void *arg);
static int ART_key_up(void *arg);
static int ART_key_dn(void *arg);
static int ART_key_lt(void *arg);
static int ART_key_rt(void *arg);
static int ART_key_er(void *arg);
static int ART_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf);
static int ART_commit(void *arg);
static void ART_Exit(void);

strategy_t	g_art_strategy = {
	ArtStrategy_entry,
	
	ART_init,
	STY_Duild_button,
	ART_key_up,
	ART_key_dn,
	ART_key_lt,
	ART_key_rt,
	ART_key_er,
	ART_get_focusdata,
	ART_commit,
	ART_Exit,
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
#define STG_SELF  g_art_strategy
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_p_art_entry[4] = {"累积算法", "流量算法", "PID算法", "变送算法"
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

static int ArtStrategy_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	if(col == 0) {
		
		if(row > 3)
			return 0;
		*pp = arr_p_art_entry[row];
		return strlen(arr_p_art_entry[row]);
	} 
	
	return 0;
}

static int ART_init(void *arg)
{
	STG_SELF.total_col = 1;
	STG_SELF.total_row = 3;
	return RET_OK;
}
static void ART_Exit(void)
{
	
}
static int ART_key_up(void *arg)
{
	return RET_OK;
}
static int ART_key_dn(void *arg)
{
	return RET_OK;
}
static int ART_key_lt(void *arg)
{
	return RET_OK;
}
static int ART_key_rt(void *arg)
{
	return RET_OK;
}
static int ART_key_er(void *arg)
{
	return RET_OK;
}

static int ART_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf)
{
	return RET_OK;
}
static int ART_commit(void *arg)
{
	return RET_OK;
}


