#include <stdint.h>
#include "HMI_striped_background.h"
#include "system.h"
#include "USB/Usb.h"
#include "os/os_depend.h"
#include "sys_cmd.h"
#include "utils/Storage.h"
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
static void DBP_build_component(void *arg);
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
	DBP_build_component,
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
 
static uint8_t		arr_DBP_fds[3];
static uint8_t		DBP_copy;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static int	DBP_Usb_event(int type);
static int DBP_update_content(int op, int weight);
static void	DBP_Btn_hdl(void *self, uint8_t	btn_id); 
 
static int	DBP_filename_commit(void *self, void *data, int len);
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
				if(arr_p_vram[row][0] != '\0')
					break;		//这种情况是从设置窗口返回，所以就不要再赋值原始值了，下面的部分也是一样
				model = ModelCreate("time");
				model->to_string(model, 1, arr_p_vram[row]);
				break;
			case 4:		
				if(arr_p_vram[row][0] == '\0')
					sprintf(arr_p_vram[row], "/CHN_%d.CSV", g_setting_chn);	//成72要求文件名必须大写
				else {
					
					if( strstr(arr_p_vram[row], ".CSV") == NULL)
					{
						
						strcat(arr_p_vram[row], ".CSV");
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
	
	
	kbr_cmt = DBP_filename_commit;

	memset(&g_DBP_strategy.sf, 0, sizeof(g_DBP_strategy.sf));
	g_DBP_strategy.sf.f_col = 1;
	g_DBP_strategy.sf.f_row = 1;
	g_DBP_strategy.sf.start_byte = 0;
	g_DBP_strategy.sf.num_byte = 1;
	HMI_Ram_init();
	for(i = 0; i < 5; i++) {
		
		arr_p_vram[i] = HMI_Ram_alloc(64);
		memset(arr_p_vram[i], 0, 64);
	}
	arr_p_vram[5] = HMI_Ram_alloc(240);		//这个缓存用于数据备份
	
	g_setting_chn = 0;
	arr_DBP_fds[0] = USB_Rgt_event_hdl(DBP_Usb_event);
//	phn_sys.key_weight = 1;
	return RET_OK;
}

static void DBP_build_component(void *arg)
{
	Button			*p_btn = BTN_Get_Sington();
	Progress_bar	*p_bar = PGB_Get_Sington();
	bar_object_t	bob = {{1, 130, 280, 16, 0, PGB_TWD_CROSS, FONT_16, PGB_TIP_RIGHT}, \
		{COLOUR_GREN, COLOUR_GRAY, COLOUR_BLUE, COLOUR_YELLOW}};
	p_btn->build_each_btn(0, BTN_TYPE_MENU, Setting_btn_hdl, arg);
	p_btn->build_each_btn(1, BTN_TYPE_COPY, DBP_Btn_hdl, arg);
	p_btn->build_each_btn(2, BTN_TYPE_STOP, DBP_Btn_hdl, arg);
		
	arr_DBP_fds[1] = p_bar->build_bar(&bob);
//	p_bar->update_bar(g_DBP_strategy.sty_some_fd, 50);
	
}
static void DBP_Exit(void)
{
	Progress_bar	*p_bar = PGB_Get_Sington();
	USB_Del_event_hdl(arr_DBP_fds[0]);
	
	//如果正在拷贝的时候，按ESC退出
	//应该让拷贝程序停止拷贝
	if(DBP_copy)
		DBP_copy = 0;
	p_bar->delete_bar(arr_DBP_fds[1]);
	kbr_cmt = NULL;
}
static int DBP_key_up(void *arg)
{
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
	int 			ret = RET_OK;
	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
//	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
//		phn_sys.key_weight += 10;
//		
//	} else {
//		phn_sys.key_weight = 1;
//	}

	


	ret = DBP_update_content(OP_ADD, phn_sys.key_weight);
	return ret;
}
static int DBP_key_dn(void *arg)
{
	
//	strategy_keyval_t	kt = {SY_KEYTYPE_HIT};
//	strategy_focus_t *p_syf = &g_sys_strategy.sf;
	int 			ret = RET_OK;
	
//	if(arg) {
//		kt.key_type = ((strategy_keyval_t *)arg)->key_type;
//		
//	}
//	if(kt.key_type == SY_KEYTYPE_LONGPUSH) {
//		phn_sys.key_weight += 10;
//		
//	} else {
//		phn_sys.key_weight = 1;
//	}
	
	ret = DBP_update_content(OP_SUB, phn_sys.key_weight);
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
		p_syf->num_byte -= strlen(".CSV");	//后缀不允许修改
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
		p_syf->num_byte -= strlen(".CSV");	//后缀不允许修改
	
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


static int	DBP_filename_commit(void *self, void *data, int len)
{
	int i = 0;
	char *p = (char *)data;
	
	//ch372要求文件名字的长度小于13个字符，其中主文件名不得超过8个字符
	if(len > 7)
		len = 7;
	
	memset(arr_p_vram[4], 0, strlen(arr_p_vram[4]));
	for(i = 0; i < len; i ++)
	{
		//把小写转化成大写
		if(p[i] < 'z' && p[i] > 'a')
		{
			p[i] -= 32;
			
		}
		arr_p_vram[4][i] = p[i];
		
	}
	return RET_OK;
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

static void DBP_Copy(void *arg)
{
	
	uint32_t			start_sec = Str_time_2_u32(arr_p_vram[2]);
	uint32_t			end_sec = Str_time_2_u32(arr_p_vram[3]);
	uint32_t			total_sec = end_sec - start_sec + 1;
	uint32_t			done_sec = 0;
	uint32_t			rd_sec = 0;
	Progress_bar	*p_bar = PGB_Get_Sington();
	char				*copy_buf = arr_p_vram[5];
	int						rd_len = 0;
	int						usb_fd = 0;
	uint32_t				dbp_count_bytes = 0;
//	usb_fd = USB_Open_file(arr_p_vram[4], USB_FM_WRITE | USB_FM_COVER);

	total_sec = 100;
	while(done_sec < total_sec)
	{

			
		if(DBP_copy == 0)
			break;
		if(phn_sys.usb_device == 0)
			goto copy_wait;
		if(usb_fd == 0)
		{
			usb_fd = USB_Create_file(arr_p_vram[4], USB_FM_WRITE | USB_FM_COVER);
			if(usb_fd == 0x42)
			{
				//todo: 文件名错误的处理要完善
				usb_fd = 0;
				goto copy_wait;
			}
		}
		
		rd_sec = 0;
		rd_len = STG_Read_rcd_by_time(g_setting_chn, start_sec, end_sec, copy_buf, 240, &rd_sec);
		if(rd_len <= 0)
			done_sec = total_sec;
		else
		{
			
			start_sec += rd_sec;
			done_sec += rd_sec;
		}
//		
		if(rd_len)
		{
			USB_Write_file(usb_fd, copy_buf, rd_len);
			dbp_count_bytes += rd_len;
		}
		
		
		//用读取的时间与总时间的比值作为进度依据
	copy_wait:
		p_bar->update_bar(arr_DBP_fds[1], done_sec * 100 / total_sec);
		delay_ms(500);
		
	}
	if(usb_fd > 0)
	{
		USB_Colse_file(usb_fd);
		//恢复出厂设置，应该不只是恢复系统设置，包括通道设置等，应该也要恢复
		sprintf(arr_p_vram[5],"完成,写入:%d B", dbp_count_bytes);
		Win_content(arr_p_vram[5]);
		g_DBP_strategy.cmd_hdl(g_DBP_strategy.p_cmd_rcv, sycmd_win_tips, NULL);
		
	}
	Cmd_del_recv(arr_DBP_fds[2]);
	
	
}

 static void	DBP_Btn_hdl(void *self, uint8_t	btn_id)
 {
	 if(btn_id == ICO_ID_COPY)
	 {
		 DBP_copy = 1;
		 arr_DBP_fds[2] = Cmd_Rgt_recv(DBP_Copy, NULL);		 
	 }
	 else if(btn_id == ICO_ID_STOP)
	 {
		 DBP_copy = 0;
	 }
	 
 }

