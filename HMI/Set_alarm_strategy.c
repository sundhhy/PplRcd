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

static int Als_entry(int row, int col, void *pp_text);
static int Als_init(void *arg);
strategy_t	g_alarm_strategy = {
	Als_entry,
	Als_init,
	
};


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
 static char *const arr_p_alarm_entry[10] = {"通道号", "报警HH", "报警HI", "报警LO", \
	 "报警LL", "触点HH", "触点HI", "触点LI", "触点LL", "报警回差"
 };
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int Als_row_aux(int row);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int Als_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	int		a_aux = 0;
	Model_chn		*p_mc = Get_Mode_chn(g_setting_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
	if(col == 0) {
		
		if(row > 9)
			return 0;
		*pp = arr_p_alarm_entry[row];
		return strlen(arr_p_alarm_entry[row]);
	} else if(col == 1) {
		if(row == 0)
			sprintf(arr_p_vram[row], "%d", g_setting_chn);
		else {
			a_aux = Als_row_aux(row);
			if(a_aux < 0)
				goto exit;
			p_md->to_string(p_md, aux, arr_p_vram[row]);
		}
		*pp = arr_p_vram[row];
		return strlen(arr_p_vram[row]);
	}
	exit:	
	return 0;
}

static int Als_init(void *arg)
{
	int i = 0;
	memset(&g_chn_strategy.sf, 0, sizeof(g_chn_strategy.sf));
	g_alarm_strategy.sf.f_col = 1;
	g_alarm_strategy.sf.f_row = 0;
	g_alarm_strategy.sf.start_byte = 0;
	g_alarm_strategy.sf.num_byte = 1;
	g_setting_chn = 0;
	VRAM_init();
	for(i = 0; i < 11; i++) {
		
		arr_p_vram[i] = VRAM_alloc(48);
		
	}
	
	g_set_weight = 1;
	return RET_OK;
}

static int Als_row_aux(int row)
{
	int ret = -1;
	switch(row)
	{
		case 1:
			ret = alarm_hh;
			break;
		case 2:
			ret = alarm_hh;
			break;
		case 3:
			ret = alarm_hh;
			break;
		case 4:
			ret = alarm_hh;
			break;
		case 5:
			ret = alarm_hh;
			break;
		case 6:
			ret = alarm_hh;
			break;
		case 7:
			ret = alarm_hh;
			break;
		case 8:
			ret = alarm_hh;
			break;
		case 9:
			ret = alarm_hh;
			break;
		case 10:
			ret = alarm_hh;
			break;
	}
	
	
	
}


