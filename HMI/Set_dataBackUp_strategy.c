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

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
 static char *const arr_p_DBU_entry[6] = {"设备当前状态：", "备份数据通道：", "起始时间：", "终止时间：",\
	 "文件名：", "备份进程"
 };
	
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	DBP_Usb_event(int type);

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
				model = ModelCreate("time");
				model->to_string(model, 1, arr_p_vram[row]);
				break;
			case 4:		//下限
				sprintf(arr_p_vram[row], "data_%d.csv", g_setting_chn);
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
	g_DBU_strategy.sty_id = DBP_ID;
	memset(&g_DBU_strategy.sf, 0, sizeof(g_DBU_strategy.sf));
	g_DBU_strategy.sf.f_col = 1;
	g_DBU_strategy.sf.f_row = 1;
	g_DBU_strategy.sf.start_byte = 0;
	g_DBU_strategy.sf.num_byte = 1;
	VRAM_init();
	for(i = 0; i < 6; i++) {
		
		arr_p_vram[i] = VRAM_alloc(64);
		memset(arr_p_vram[i], 0, 64);
	}
	g_setting_chn = 0;
	g_DBU_strategy.sty_some_fd = USB_Rgt_event_hdl(DBP_Usb_event);
//	g_set_weight = 1;
	return RET_OK;
}
static void DBP_Exit(void)
{
	USB_Del_event_hdl(g_DBU_strategy.sty_some_fd);
	
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
	
	strategy_focus_t *p_syf = &g_DBU_strategy.sf;
	char		**pp_vram = (char **)pp_data;
	int ret = 0;
	
	if((p_syf->f_row < 1) || (p_syf->f_row > 4)) {
		return -1;
	}
	
	if(p_in_syf)
		p_syf = p_in_syf;
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
	
	if(phn_sys.hmi_mgr.set_strategy != DBP_ID)
		return RET_OK;
	if((type != et_ready) && (type != et_remove))
		return RET_OK;
	g_DBU_strategy.cmd_hdl(g_DBU_strategy.p_cmd_rcv, sycmd_reflush, NULL);
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

