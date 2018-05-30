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
static void ALS_build_component(void *arg);

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
	ALS_build_component,
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
enum {
	row_chn_num,
	row_alarm_hh,
	row_alarm_hi,
	row_alarm_lo,
	row_alarm_ll,
	row_tchspt_hh,
	row_tchspt_hi,
	row_tchspt_lo,
	row_tchspt_ll,
	row_alarm_backlash,
	num_rows
}als_rows;




#define STG_NUM_VRAM			(num_rows + 1)
#define STG_RUN_VRAM_NUM		num_rows
#define STG_SELF  g_alarm_strategy
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	char			cur_chn;
	char		none[3];
	char 		win_buf[48];
	chn_alarm_t	tmp_alarm[NUM_CHANNEL];
	int			arr_flag_change[NUM_CHANNEL];
}als_run_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_p_alarm_entry[num_rows] = {"通道号", "报警HH", "报警HI", "报警LO", \
	 "报警LL", "触点HH", "触点HI", "触点LI", "触点LL", "报警回差"
 };
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int Als_row_aux(int row);
static int Als_modify(void *arg, int op);
static void ALS_Set_mdl_tmp_alarm(int chn, chn_alarm_t *p_alarm);		

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
	als_run_t	*p_run = (als_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	int		a_aux = 0;
	Model_chn		*p_mc ;
	Model				*p_md;
	
	p_mc = Get_Mode_chn(p_run->cur_chn);
	p_md = SUPER_PTR(p_mc, Model);
	if(col == 0) {
		
		if(row > (num_rows - 1))
			return 0;
		*pp = arr_p_alarm_entry[row];
		return strlen(arr_p_alarm_entry[row]);
	} else if(col == 1) {
		if(row == 0)
			sprintf(arr_p_vram[row], "%d", p_run->cur_chn);
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
	als_run_t *p_run;
	memset(&g_chn_strategy.sf, 0, sizeof(g_chn_strategy.sf));
	g_alarm_strategy.sf.f_col = 1;
	g_alarm_strategy.sf.f_row = 0;
	g_alarm_strategy.sf.start_byte = 0;
	g_alarm_strategy.sf.num_byte = 1;
	
	
	HMI_Ram_init();
	for(i = 0; i < STG_NUM_VRAM; i++) {
		
		
		
		if(i != STG_RUN_VRAM_NUM)
		{
			arr_p_vram[i] = HMI_Ram_alloc(48);
			memset(arr_p_vram[i], 0, 48);
		}
		else
		{
			arr_p_vram[i] = HMI_Ram_alloc(sizeof(als_run_t));
			memset(arr_p_vram[i], 0, sizeof(als_run_t));
		}
			
	}
	STG_SELF.total_col = 2;
	STG_SELF.total_row = num_rows;
	phn_sys.key_weight = 1;
	
	p_run = (als_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	for(i = 0; i < NUM_CHANNEL; i++)
		ALS_Set_mdl_tmp_alarm(i, &p_run->tmp_alarm[i]);
	
	return RET_OK;
}
static void ALS_Exit(void)
{
	int i;
	for(i = 0; i < NUM_CHANNEL; i++)
		ALS_Set_mdl_tmp_alarm(i, NULL);
}
static int ALS_commit(void *arg)
{
	return RET_OK;
	
}
static void ALS_Set_mdl_tmp_alarm(int chn, chn_alarm_t *p_alarm)
{
	als_run_t *p_run = (als_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	
	Model_chn			*p_mc = Get_Mode_chn(chn);
	
	
	MCH_Set_alarm_buf(p_mc, p_alarm, sizeof(chn_alarm_t));
	
	
}

 static void	ALS_Btn_hdl(void *self, uint8_t	btn_id)
 {
	 short		i;
	 short 		flag = 0;
	 als_run_t *p_run = (als_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	 if(btn_id == BTN_TYPE_SAVE)
	 {
		for(i = 0; i < NUM_CHANNEL; i++)
		{
			if(p_run->arr_flag_change[i])
			{
				
				
				
				if(MdlChn_Commit_conf(i) == RET_OK)
				{
					flag = 1;
					p_run->arr_flag_change[i] = 0;
					
//					sprintf(p_run->win_buf,"写入配置成功");
//					Win_content(p_run->win_buf);
//					STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				else
				{
					flag = 2;
					break;
//					sprintf(p_run->win_buf,"通道[%d] 写入配置失败", i);
//					Win_content(p_run->win_buf);
//					STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
				}
				
			}
			
			
		}	
		
		
		if(flag == 1)
		{
			sprintf(p_run->win_buf,"写入配置成功");
			Win_content(p_run->win_buf);
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
			
		}
		else if(flag == 2)
		{
			
			sprintf(p_run->win_buf,"通道[%d] 写入配置失败", i);
			Win_content(p_run->win_buf);
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
		}

		 
	 }
	
	 
 }

static void ALS_build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(3, BTN_TYPE_SAVE, ALS_Btn_hdl, arg);
		
	
	
}

static int Als_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &g_alarm_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_syf->f_row > (num_rows - 1)) {
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
//		p_syf->f_row = (num_rows - 1);
		p_syf->f_row = 0;
		ret = -1;
		
	}
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	return ret;
}

static int Als_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &g_alarm_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row < (num_rows - 1))
		p_syf->f_row ++;
	else {
		p_syf->f_row = 0;
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
		case row_alarm_hh:
			ret = alarm_hh;
			break;
		case row_alarm_hi:
			ret = alarm_hi;
			break;
		case row_alarm_lo:
			ret = alarm_lo;
			break;
		case row_alarm_ll:
			ret = alarm_ll;
			break;
		case row_tchspt_hh:
			ret = tchspt_hh;
			break;
		case row_tchspt_hi:
			ret = tchspt_hi;
			break;
		case row_tchspt_lo:
			ret = tchspt_lo;
			break;
		case row_tchspt_ll:
			ret = tchspt_ll;
			break;
		case row_alarm_backlash:
			ret = alarm_backlash;
			break;

	}
	
	return ret;
	
}

static int Als_modify(void *arg, int op)
{
	
	int					a_aux = 0;
	als_run_t			*p_run = (als_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	Model_chn			*p_mc;
	Model				*p_md;
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	strategy_focus_t 	*p_syf = &g_alarm_strategy.sf;
	int 				ret = RET_OK;
	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
	p_mc = Get_Mode_chn(p_run->cur_chn);
	p_md = SUPER_PTR(p_mc, Model);

	
	
	if(p_syf->f_row == row_chn_num) {
		p_run->cur_chn = Operate_in_range(p_run->cur_chn, op, 1, 0, phn_sys.sys_conf.num_chn - 1);
		g_alarm_strategy.cmd_hdl(g_alarm_strategy.p_cmd_rcv, sycmd_reflush, NULL);
		
	} else {
		a_aux = Als_row_aux(p_syf->f_row);
		if(a_aux < 0)
			goto exit;
		p_run->arr_flag_change[p_run->cur_chn] = 1;
		p_md->modify_str_conf(p_md, a_aux, arr_p_vram[p_syf->f_row], op, phn_sys.key_weight);
		p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	}
	
	exit:
	return ret;
	
}
