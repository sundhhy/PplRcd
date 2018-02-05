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

static int Als_entry(int row, int col, void *pp_text);
static int Als_init(void *arg);
static int Als_key_up(void *arg);
static int Als_key_dn(void *arg);
static int Als_key_lt(void *arg);
static int Als_key_rt(void *arg);
static int Als_key_er(void *arg);
static int Als_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
static int ALS_commit(void *arg);
static void ALS_Exit(void);

strategy_t	g_alarm_strategy = {
	Als_entry,
	Als_init,
	STY_Duild_button,
	Als_key_up,
	Als_key_dn,
	Als_key_lt,
	Als_key_rt,
	Als_key_er,
	Als_get_focusdata,	
	ALS_commit,
	ALS_Exit,
};



//static int Cns_init(void *arg);
//static int Cns_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf);
//strategy_t	g_chn_strategy = {
//	ChnStrategy_entry,
//	Cns_init,
//	Cns_key_up,
//	Cns_key_dn,
//	Cns_key_lt,
//	Cns_key_rt,
//	Cns_key_er,
//	Cns_get_focusdata,
//};
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
 static int Als_modify(void *arg, int op);
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
			p_md->to_string(p_md, a_aux, arr_p_vram[row]);
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
	
	HMI_Ram_init();
	for(i = 0; i < 10; i++) {
		
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	
	phn_sys.key_weight = 1;
	return RET_OK;
}
static void ALS_Exit(void)
{
	
}
static int ALS_commit(void *arg)
{
	return RET_OK;
	
}

static int Als_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &g_alarm_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_syf->f_row > 9) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
	ret = p_syf->num_byte;
	
	
	
	*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;

	return ret;
	
}

static int Als_key_up(void *arg)
{
	return Als_modify(arg, OP_ADD);
}
static int Als_key_dn(void *arg)
{
	return Als_modify(arg, OP_SUB);
}
static int Als_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &g_alarm_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row )
		p_syf->f_row --;
	else {
		p_syf->f_row = 9;
		ret = -1;
		
	}
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	return ret;
}

static int Als_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_alarm_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row < 9)
		p_syf->f_row ++;
	else {
		p_syf->f_row = 0;
		p_syf->f_col = 1;
		ret = -1;
	}
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);	
	return ret;
}
static int Als_key_er(void *arg)
{
	return 0;
	
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
			ret = alarm_hi;
			break;
		case 3:
			ret = alarm_lo;
			break;
		case 4:
			ret = alarm_ll;
			break;
		case 5:
			ret = tchspt_hh;
			break;
		case 6:
			ret = tchspt_hi;
			break;
		case 7:
			ret = tchspt_lo;
			break;
		case 8:
			ret = tchspt_ll;
			break;
		case 9:
			ret = alarm_backlash;
			break;

	}
	
	return ret;
	
}

static int Als_modify(void *arg, int op)
{
	
	int					a_aux = 0;
	Model_chn			*p_mc = Get_Mode_chn(g_setting_chn);
	Model				*p_md = SUPER_PTR(p_mc, Model);
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	strategy_focus_t 	*p_syf = &g_alarm_strategy.sf;
	int 				ret = RET_OK;
	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
	


	
	
	if(p_syf->f_row == 0) {
		g_setting_chn = Operate_in_tange(g_setting_chn, op, 1, 0, NUM_CHANNEL - 1);
		g_alarm_strategy.cmd_hdl(g_alarm_strategy.p_cmd_rcv, sycmd_reflush, NULL);
		
	} else {
		a_aux = Als_row_aux(p_syf->f_row);
		if(a_aux < 0)
			goto exit;
		p_md->modify_str_conf(p_md, a_aux, arr_p_vram[p_syf->f_row], op, phn_sys.key_weight);
		p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	}
	
	exit:
	return ret;
	
}
