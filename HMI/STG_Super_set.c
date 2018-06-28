#include "HMI_striped_background.h"
#include "sdhDef.h"
#include "ModelFactory.h"
#include "system.h"
#include <string.h>
#include "utils/log.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
/*
超级密码输入时进入
可以设置超级密码，以及通道数量

*/
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------
static int SPR_Entry(int row, int col, void *pp_text);
static void SPR_build_component(void *arg);

static int SPR_key_up(void *arg);
static int SPR_key_dn(void *arg);
static int SPR_key_lt(void *arg);
static int SPR_key_rt(void *arg);
static int SPR_key_er(void *arg);
static int SPR_init(void *arg);
static int SPR_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf);
static int SPR_commit(void *arg);
static void SYS_Exit(void);

strategy_t	g_stg_super = {
	SPR_Entry,
	SPR_init,
	SPR_build_component,
	SPR_key_up,
	SPR_key_dn,
	SPR_key_lt,
	SPR_key_rt,
	SPR_key_er,
	SPR_get_focusdata,
	SPR_commit,
	SYS_Exit,

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
enum {
	row_set_super_psd,
	num_row_chn,
	row_stg_alarm,
	num_row
	
	
}supre_rows;

#define STG_NUM_VRAM			(num_row)
#define STG_RUN_VRAM_NUM	 num_row
#define STG_SELF						g_stg_super

#define STG_ALARM_LOW				1000
#define STG_ALARM_HIG				1000000
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint8_t		super_psd[3];
	uint8_t		tmp_num_channel;
	
	uint32_t	storage_alarm;	
	char			tip_buf[32];
}suprt_run_t;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

 static char *const arr_p_sys_entry[num_row] = {"超级密码", "通道数目","剩余不足报警"};
 
#define STG_P_RUN		(suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
#define INIT_RUN_RAM do { \
	arr_p_vram[STG_RUN_VRAM_NUM] = HMI_Ram_alloc(sizeof(supre_rows)); \
	memset(arr_p_vram[STG_RUN_VRAM_NUM], 0, sizeof(supre_rows)); \
}while(0)
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void SPR_update_syf(strategy_focus_t *p_syf);
static int SPR_update_content(int op, int weight);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int SPR_Entry(int row, int col, void *pp_text)
{
	char 	**pp = (char **)pp_text;
//	Model	*model;
	suprt_run_t	*p_run = (suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
//	strategy_focus_t *p_syf = &STG_SELF.sf;
	
	
	
	if(row >= num_row)
			return 0;
	if(col == 0) {
		
		*pp = arr_p_sys_entry[row];
		return strlen(arr_p_sys_entry[row]);
	} else if(col == 1){
		
		
		switch(row) {
			
			case row_set_super_psd:
				Print_sys_param(p_run->super_psd, arr_p_vram[row], 48, es_psd);
				break;
			case num_row_chn:
				sprintf(arr_p_vram[row], "%d", p_run->tmp_num_channel);
				break;
			case row_stg_alarm:
				sprintf(arr_p_vram[row], "%d", p_run->storage_alarm);
				break;
			
			default:
				goto exit;
		}
		
		
		*pp = arr_p_vram[row];
		return strlen(*pp);
		
	}
	exit:
	return 0;
}

static int SPR_init(void *arg)
{
	int i = 0;
	suprt_run_t *p_run;
	memset(&STG_SELF.sf, 0, sizeof(STG_SELF.sf));
	STG_SELF.sf.f_col = 1;
	STG_SELF.sf.f_row = 0;
	SPR_update_syf(&STG_SELF.sf);
	
	HMI_Ram_init();
	
	//第14个用于提示显示
	for(i = 0; i < STG_NUM_VRAM; i++) {
		
		arr_p_vram[i] = HMI_Ram_alloc(48);
		memset(arr_p_vram[i], 0, 48);
	}
	STG_SELF.total_col = 2;
	STG_SELF.total_row = num_row;
	
	
	
	INIT_RUN_RAM;
	p_run = STG_P_RUN;
	p_run->tmp_num_channel = phn_sys.sys_conf.num_chn;
	p_run->storage_alarm = phn_sys.sys_conf.storage_alarm;
	Clone_psd(phn_sys.sys_conf.super_psd, p_run->super_psd);
	return RET_OK;
}
static void SYS_Exit(void)
{
	
}
static int SPR_get_focusdata(void *pp_data, strategy_focus_t *p_in_syf)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if(p_in_syf)
		p_syf = p_in_syf;
	
	
	if(p_syf->f_row < num_row) {
		*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
		p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
		ret = p_syf->num_byte;
	}
	else 
		ret = -1;
	
	
	return ret;
	
}



static int SPR_key_up(void *arg)
{
	return SPR_update_content(OP_ADD, phn_sys.key_weight);
}


 static void	SPR_btn_hdl(void *self, uint8_t	btn_id)
 {
	 
	 suprt_run_t	*p_run = (suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	 if(btn_id == BTN_TYPE_SAVE)
	 {
		//保存超级设置
		Clone_psd(p_run->super_psd, phn_sys.sys_conf.super_psd);
		phn_sys.sys_conf.num_chn = p_run->tmp_num_channel;
		phn_sys.sys_conf.storage_alarm = p_run->storage_alarm; 
		 
		SYS_Commit();		//保存到flash
		 
		sprintf(p_run->tip_buf,"系统设置写入成功");
		Win_content(p_run->tip_buf);
		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, NULL);
			
			
	}
			
		
 }
static void SPR_build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(3, BTN_TYPE_SAVE, SPR_btn_hdl, arg);
		
	
	
}
static int SPR_key_dn(void *arg)
{
	


	


	return SPR_update_content(OP_SUB, phn_sys.key_weight);


	
	
}

static int SPR_key_rt(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
//	suprt_run_t	*p_run = (suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	int ret = RET_OK;

	
		
	p_syf->f_row = Operate_in_range(p_syf->f_row, OP_ADD, 1, 0, num_row);
	
	if(p_syf->f_row == num_row)
	{
		//说明光标发生了反转
		//只有超过范围才会反转
		p_syf->f_row  = 0;
		ret = -1;
	} 
	
	
	
	SPR_update_syf(p_syf);
	return ret;
}

static int SPR_key_lt(void *arg)
{
	strategy_focus_t *p_syf = &STG_SELF.sf;
//	suprt_run_t	*p_run = (suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	int ret = RET_OK;

	
	
	if(p_syf->f_row == 0)
	{
		//说明光标发生了反转
		//只有超过范围才会反转
		ret = -1;
	}
	p_syf->f_row = Operate_in_range(p_syf->f_row, OP_SUB, 1, 0, num_row - 1);
		
		
	
	SPR_update_syf(p_syf);
	return ret;
}



static int SPR_key_er(void *arg)
{

//	strategy_focus_t 	*p_syf = &STG_SELF.sf;
//	int								ret = RET_OK;
//	switch(p_syf->f_row) {
//	
//	case row_set_super_psd:
//		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_psd, arr_p_vram[p_syf->f_row]);
//		break;
//	case num_row_chn:
//		
//		STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_tips, arr_p_vram[p_syf->f_row]);
//		
//		break;
//	default:
//		ret = ERR_OPT_FAILED;
//		break;
//	
//	
//	}
	

	
	
	return ERR_OPT_FAILED;
}

static int SPR_commit(void *arg)
{
	//将所有的配置项写入模型

	strategy_focus_t 	*p_syf = &STG_SELF.sf;
	suprt_run_t			*p_run = (suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
//	int								ret = RET_OK;
	
	
	
	switch(p_syf->f_row) {
	
		case row_set_super_psd:
			Str_set_password(arr_p_vram[p_syf->f_row], p_run->super_psd);
			break;
	
	
	
	}

//	
	
	return RET_OK;
	
}

static void SPR_update_syf(strategy_focus_t *p_syf)
{
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	
//	//把单位剔除掉
//	switch(p_syf->f_row)
//	{
//		case 6:		//x M
//		case 7:		//x S
//		case 8:		//x %
//			p_syf->num_byte -= 2;
//			break;
//		
//	}
//	
	
}


static int SPR_update_content(int op, int weight)
{
	strategy_focus_t 	*p_syf = &STG_SELF.sf;
	int					ret = RET_OK;
	suprt_run_t	*p_run = (suprt_run_t *)arr_p_vram[STG_RUN_VRAM_NUM];
	
	switch(p_syf->f_row) {
	
		case row_set_super_psd:
			STG_SELF.cmd_hdl(STG_SELF.p_cmd_rcv, sycmd_win_psd, arr_p_vram[p_syf->f_row]);
			ret = 1;
			break;
		case num_row_chn:		
			p_run->tmp_num_channel = Operate_in_range(p_run->tmp_num_channel, op, 1,1, NUM_CHANNEL);
			sprintf(arr_p_vram[p_syf->f_row], "%d", p_run->tmp_num_channel);
			break;
		case row_stg_alarm:		
			p_run->storage_alarm = Operate_in_range_keep(p_run->storage_alarm, op, weight,STG_ALARM_LOW, STG_ALARM_HIG);
			sprintf(arr_p_vram[p_syf->f_row], "%d", p_run->storage_alarm);
			break;
	
		default:
			break;
	
	
	}
	
	SPR_update_syf(p_syf);
	
	return ret;
}

