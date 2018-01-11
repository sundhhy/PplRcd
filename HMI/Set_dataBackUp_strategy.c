#include <stdint.h>
#include "Setting_HMI.h"
#include "system.h"
#include "USB/Usb.h"
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define DBP_ID		0x01
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
static void DBP_Exit(void);
strategy_t	g_DBP_strategy = {
	Data_bacnup_Strategy_entry,
	DBP_init,
	DBP_key_up,
	DBP_key_dn,
	DBP_key_lt,
	DBP_key_rt,
	DBP_key_er,
	DBP_get_focusdata,
	DBP_commit,
	DBP_Exit,
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
CLASS(DBP_btn_recv)
{
	IMPLEMENTS(Button_receive);
	
};
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_p_DBU_entry[6] = {"设备当前状态：", "备份数据通道：", "起始时间：", "终止时间：",\
	 "文件名：", "备份进程"
 };
 
 static Button_receive	*dbp_btn_rcv;
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	DBP_Usb_event(int type);
static int DBP_update_content(int op, int weight);
 
 static void	DBP_Btn_recv(Button_receive *self, uint8_t	btn_id);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

 

 
 
CTOR(DBP_btn_recv)
FUNCTION_SETTING(Button_receive.btn_recv, DBP_Btn_recv);

END_CTOR
 
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//

static int Data_bacnup_Strategy_entry(int row, int col, void *pp_text)
{
	char **pp = (char **)pp_text;
	Model	*model;
	if(col == 0) {
		
		if(row > 5)
			return 0;
		*pp = arr_p_DBU_entry[row];
		return strlen(arr_p_DBU_entry[row]);
	} 
	else if(col == 1)
	{
		switch(row) 
		{
			case 0:
				if(phn_sys.usb_device)
					sprintf(arr_p_vram[row], "设备已连接");
				else
					sprintf(arr_p_vram[row], "设备未连接");
				break;
			case 1:		//
				sprintf(arr_p_vram[row], "%d", g_setting_chn);
				break;
			case 2:	
			case 3:		//单位
				if(arr_p_vram[row][0] != '\0')
					break;		//这种情况是从设置窗口返回，所以就不要再赋值原始值了，下面的部分也是一样
				model = ModelCreate("time");
				model->to_string(model, 1, arr_p_vram[row]);
				break;
			case 4:		
				if(arr_p_vram[row][0] == '\0')
					sprintf(arr_p_vram[row], "data_%d.csv", g_setting_chn);
				else {
					
					if( strstr(arr_p_vram[row], ".csv") == NULL)
					{
						
						strcat(arr_p_vram[row], ".csv");
					}
					
				}
				break;
			default:
				goto exit;
		}
		*pp = arr_p_vram[row];
		return strlen(arr_p_vram[row]);
		
	}
	exit:
	return 0;
}

static int DBP_init(void *arg)
{
	int			i;
	
	if(dbp_btn_rcv == NULL)
		dbp_btn_rcv = SUPER_PTR(DBP_btn_recv_new(), Button_receive);
	
	g_DBP_strategy.sty_id = DBP_ID;
	memset(&g_DBP_strategy.sf, 0, sizeof(g_DBP_strategy.sf));
	g_DBP_strategy.sf.f_col = 1;
	g_DBP_strategy.sf.f_row = 1;
	g_DBP_strategy.sf.start_byte = 0;
	g_DBP_strategy.sf.num_byte = 1;
	VRAM_init();
	for(i = 0; i < 6; i++) {
		
		arr_p_vram[i] = VRAM_alloc(64);
		memset(arr_p_vram[i], 0, 64);
	}
	g_setting_chn = 0;
	g_DBP_strategy.sty_some_fd = USB_Rgt_event_hdl(DBP_Usb_event);
//	g_set_weight = 1;
	return RET_OK;
}

static void DBP_Init_button(void)
{
	Button	*p_btn = BTN_Get_Sington();
	p_btn->build_each_btn(0, BTN_TYPE_MENU, dbp_btn_rcv);
	p_btn->build_each_btn(1, BTN_TYPE_COPY, dbp_btn_rcv);
	
}
static void DBP_Exit(void)
{
	USB_Del_event_hdl(g_DBP_strategy.sty_some_fd);
	
}
static int DBP_key_up(void *arg)
{
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	int 			ret = RET_OK;
	
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		g_set_weight += 10;
		
	} else {
		g_set_weight = 1;
	}

	


	ret = DBP_update_content(OP_ADD, g_set_weight);
	return ret;
}
static int DBP_key_dn(void *arg)
{
	
	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int 			ret = RET_OK;
	
	if(arg) {
		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
		
	}
	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
		g_set_weight += 10;
		
	} else {
		g_set_weight = 1;
	}
	
	ret = DBP_update_content(OP_SUB, g_set_weight);
	return ret;
}
static int DBP_key_lt(void *arg)
{
	
	strategy_focus_t *p_syf = &g_DBP_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row > 1)
		p_syf->f_row --;
	else {
		p_syf->f_row = 4;
		ret = -1;
		
	}
	
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	if(p_syf->f_row == 4)
		p_syf->num_byte -= strlen(".csv");	//后缀不允许修改
	return ret;
}
static int DBP_key_rt(void *arg)
{
	
	strategy_focus_t *p_syf = &g_DBP_strategy.sf;
	int ret = RET_OK;
	
	if(p_syf->f_row < 4)
		p_syf->f_row ++;
	else {
		p_syf->f_row = 1;
		p_syf->f_col = 1;
		ret = -1;
	}
	
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	if(p_syf->f_row == 4)
		p_syf->num_byte -= strlen(".csv");	//后缀不允许修改
	return ret;
}
static int DBP_key_er(void *arg)
{
	return RET_OK;
}

static int DBP_get_focusdata(void *pp_data,  strategy_focus_t *p_in_syf)
{
	
	strategy_focus_t *p_syf = &g_DBP_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if((p_syf->f_row < 1) || (p_syf->f_row > 4)) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
	
	
	
	
	
	p_syf->num_byte = strlen(arr_p_vram[p_syf->f_row]);
	if(p_syf->f_row == 4)
		p_syf->num_byte -= strlen(".csv");	//后缀不允许修改
	
	ret = p_syf->num_byte;
	*pp_vram = arr_p_vram[p_syf->f_row] + p_syf->start_byte;
	return ret;
}
static int DBP_commit(void *arg)
{
	return RET_OK;
}


static int	DBP_Usb_event(int type)
{
	strategy_focus_t		pos;
	if(phn_sys.hmi_mgr.set_strategy != DBP_ID)
		return RET_OK;
	if((type != et_ready) && (type != et_remove))
		return RET_OK;
	
	pos.f_col = 1;
	pos.f_row = 0;
	g_DBP_strategy.cmd_hdl(g_DBP_strategy.p_cmd_rcv, sycmd_reflush_position, &pos);
//	if(type == et_ready)
//	{
//		
//		
//	}
//	else if(type == et_remove)
//	{
//		
//		
//	}
	
	return 0;
}

static int DBP_update_content(int op, int weight)
{
	strategy_focus_t 	*p_syf = &g_DBP_strategy.sf;
	int					ret = RET_OK;
	switch(p_syf->f_row) {
		case 1:
			g_setting_chn = Operate_in_tange(g_setting_chn, op, 1, 0, NUM_CHANNEL - 1);
			sprintf(arr_p_vram[1], "%d", g_setting_chn);
			break;
		case 2:		
//			g_DBP_strategy.cmd_hdl(g_sys_strategy.p_cmd_rcv, sycmd_win_time, arr_p_vram[p_syf->f_row]);
//			ret = 1;
//			break;
		case 3:
			g_DBP_strategy.cmd_hdl(g_DBP_strategy.p_cmd_rcv, sycmd_win_time, arr_p_vram[p_syf->f_row]);
			ret = 1;
			break;
		case 4:
			g_DBP_strategy.cmd_hdl(g_DBP_strategy.p_cmd_rcv, sycmd_keyboard, arr_p_vram[p_syf->f_row]);
			ret = 1;
			break;
	default:
		break;
	
	
	}
	return ret;
}

 static void	DBP_Btn_recv(Button_receive *self, uint8_t	btn_id)
 {
	 
	 
 }

