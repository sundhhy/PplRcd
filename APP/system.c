//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "HMI/HMIFactory.h"

#include "system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sdhDef.h"
#include "utils/rtc_pcf8563.h"
#include "utils/hw_w25q.h"
#include "utils/FM25.h"


#include "fs/easy_fs.h"
#include "utils/Storage.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define 	PHN_MAJOR_VER				0
#define 	PHN_MINOR_VER				11
//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static UtlRtc *sys_rtc;	
//为了能够紧凑的定义一些静态变量，所以都定义在一起
//只有非4字节对齐的，需要放在这里定于
char 				*arr_p_vram[16];
uint16_t		next_record;
char				g_setting_chn = 0;
char				flush_flag = 0;
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------
system_t		phn_sys;
//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
static const int	arr_baud[7] = {1200, 4800, 9600, 19200, 38400, 57200, 115200};
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Break_deal_string(char *p, int deal);
static void Disable_string(char *p, int able);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//



void System_default(void)
{
	system_conf_t 	*p_sc = &phn_sys.sys_conf;
	Storage			*stg = Get_storage();
	int				i = 0;
	
	memset(p_sc, 0, sizeof(system_conf_t));
	p_sc->sys_flag = 0;
	p_sc->num_chn = NUM_CHANNEL;
	stg->wr_stored_data(stg, CFG_TYPE_SYSTEM, &phn_sys.sys_conf);
	
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		
		MdlChn_default_conf(i);
		MdlChn_default_alarm(i);
		stg->wr_stored_data(stg, CFG_CHN_CONF(i), NULL);
	}
	
//	p_sc->baud_idx = 0;
//	p_sc->baud_rate = arr_baud[0];
//	p_sc->disable_view_chn_status = 0;
}

void System_init(void)
{
	struct  tm stm;
	Model 		*md_time;
	Storage					*stg = Get_storage();


	phn_sys.major_ver = PHN_MAJOR_VER;
	phn_sys.minor_ver = PHN_MINOR_VER;
	
	sys_rtc = ( UtlRtc *)Pcf8563_new();
	sys_rtc->init(sys_rtc, NULL);
	sys_rtc->get(sys_rtc, &stm);
	
	//md_time要系统时间初始化之后初始化
	md_time = ModelCreate("time");
	md_time->init(md_time, NULL);
	
	w25q_init();
	FM25_init();
	EFS_init(NUM_FSH);
	stg->init(stg);
	
	stg->rd_stored_data(stg, CFG_TYPE_SYSTEM, &phn_sys.sys_conf);
	if(phn_sys.sys_conf.num_chn != NUM_CHANNEL)
		System_default();
		
	
}
void System_time(struct  tm *stime)
{
	
	sys_rtc->get(sys_rtc, stime);
}

extern uint32_t System_tm_2_u32(struct  tm *stime)
{
	
	return 0;
}
extern int System_u32_2_tm(uint32_t time_u32, struct  tm *stime)
{
	
	return RET_OK;
}

int  System_set_time(struct  tm *stime)
{
	int ret;
	ret = sys_rtc->set(sys_rtc, stime);
	
	if(ret == RET_OK)
		sys_rtc->get(sys_rtc, stime);
	
	return ret;
}

//"** ** **"
void Password_modify(char	*p_s_psd, int idx, int op)
{
	
	switch(idx) {
		case 0:		
		case 1:
		case 3:
		case 4:		
		case 6:
		case 7:
			Str_Calculations(p_s_psd + idx, 1, op, 1, 0, 9);
			break;
		default:
			break;
	}
	
	
}

int Str_Password_match(char *p_s_psd)
{
	
	
	short	i, data;
	int			ret = 0;
	
	for(i = 0; i < 3; i++) {
		data = atoi(p_s_psd);
	
		if( phn_sys.sys_conf.password[i] != data) {
			ret = 1;
			break;
		}
		
		p_s_psd += 3;
	}
	
	return ret;
	
}

//"** ** **"
void Password_set_by_str(char	*p_s_psd)
{
	short	i, data;
	
	for(i = 0; i < 3; i++) {
		data = atoi(p_s_psd);
	
		phn_sys.sys_conf.password[i] = data;
		
		p_s_psd += 3;
	}
	phn_sys.save_chg_flga |= CHG_SYSTEM_CONF;
	
	
}

//对时间显示的字符上移动
//返回新的位置
int Password_iteartor(char	*p_time_text, int idx, int director)
{
	int	new_idx = 0;
	if(director == 0) {
		//左移
		if(idx == 0)
			new_idx = 7;
		else
			new_idx = idx - 1;
		//跳过非数字字符
		while(p_time_text[new_idx] > '9' || p_time_text[new_idx] < '0')
			new_idx --;
		
		
	} else {
		//右移
		if(idx >= 7)
			new_idx = 0;
		else
			new_idx = idx + 1;
		//跳过非数字字符
		while(p_time_text[new_idx] > '9' || p_time_text[new_idx] < '0')
			new_idx ++;
	
		
	}
	
	return new_idx;
	
}


void System_to_string(void *p_data, char	*p_s, int len, int aux)
{
	uint8_t *p_u8;
	switch(aux)
	{
		case es_psd:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = phn_sys.sys_conf.password;
			
			sprintf(p_s, "%02d %02d %02d", p_u8[0], p_u8[1], p_u8[2]);
			break;
		
		case es_brk_cpl:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.break_couple;
			
			Break_deal_string(p_s, *p_u8);
			break;
		case es_brk_rss:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.break_resistor;
			
			Break_deal_string(p_s, *p_u8);
			break;
		case es_baud:
			
			break;
		case es_cmn_md:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.communication_mode;
			
			if(*p_u8 == 0)
				sprintf(p_s, "通讯");
			else 
				sprintf(p_s, "打印");
			break;
		case es_mdfy_prm:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.disable_modify_adjust_paramter;
			
			Disable_string(p_s, *p_u8);
			break;
			
		case es_CJC:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.CJC;
			if(*p_u8 < 100) {
				
				sprintf(p_s, "设定: %d", *p_u8);
			} else {
				sprintf(p_s, "外部    ");
			}
			break;
			
		case es_vcs:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.disable_view_chn_status;
			
			Disable_string(p_s, *p_u8);
			break;
		case es_beep:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.enable_beep;
			
			Disable_string(p_s, *p_u8);
			break;
	}
	
	
}



void System_modify_string(char	*p_s, int aux, int op, int val)
{
	phn_sys.save_chg_flga |= CHG_SYSTEM_CONF;
	switch(aux)
	{
		case es_rcd_t_s:
			phn_sys.sys_conf.record_gap_s = Operate_in_tange(phn_sys.sys_conf.record_gap_s, op, val, 0, 99);
			next_record = phn_sys.sys_conf.record_gap_s;
			sprintf(p_s, "%d", phn_sys.sys_conf.record_gap_s);
			break;
		case es_brk_cpl:
			
			phn_sys.sys_conf.break_couple = Operate_in_tange(phn_sys.sys_conf.break_couple, op, val, 0, 2);
			Break_deal_string(p_s, phn_sys.sys_conf.break_couple);
			break;
		case es_brk_rss:
			phn_sys.sys_conf.break_resistor = Operate_in_tange(phn_sys.sys_conf.break_resistor, op, val, 0, 2);
			Break_deal_string(p_s, phn_sys.sys_conf.break_resistor);
			break;
		case es_baud:
			phn_sys.sys_conf.baud_idx = Operate_in_tange(phn_sys.sys_conf.baud_idx, op, val, 0, 6);
			phn_sys.sys_conf.baud_rate = arr_baud[phn_sys.sys_conf.baud_idx];
			sprintf(p_s, "%d", phn_sys.sys_conf.baud_rate);
			break;
		case es_id:
			phn_sys.sys_conf.id = Operate_in_tange(phn_sys.sys_conf.id, op, val, 1, 63);
			sprintf(p_s, "%d", phn_sys.sys_conf.id);
			break;
		case es_cmn_md:
			phn_sys.sys_conf.communication_mode = Operate_in_tange(phn_sys.sys_conf.communication_mode, op, val, 0, 1);
			if(phn_sys.sys_conf.communication_mode == 0)
				sprintf(p_s, "通讯");
			else 
				sprintf(p_s, "打印");
			break;
		case es_mdfy_prm:
		
			phn_sys.sys_conf.disable_modify_adjust_paramter = Operate_in_tange(phn_sys.sys_conf.disable_modify_adjust_paramter, op, val, 0, 1);
			Disable_string(p_s, phn_sys.sys_conf.disable_modify_adjust_paramter );
			break;
			
		case es_CJC:
			phn_sys.sys_conf.CJC = Operate_in_tange(phn_sys.sys_conf.CJC, op, val, 0, 100);
			System_to_string(NULL, p_s, 0xff, es_CJC);
//			if(phn_sys.sys_conf.CJC < 100) {
//				
//				sprintf(p_s, "设定 %d", phn_sys.sys_conf.CJC);
//			} else {
//				sprintf(p_s, "外部    ");
//			}
			break;
			
			
		case es_vcs:
			phn_sys.sys_conf.disable_view_chn_status = Operate_in_tange(phn_sys.sys_conf.disable_view_chn_status, op, val, 0, 1);
			Disable_string(p_s, phn_sys.sys_conf.disable_view_chn_status);
			break;
		case es_beep:
			phn_sys.sys_conf.enable_beep = Operate_in_tange(phn_sys.sys_conf.enable_beep, op, val, 0, 1);
			Disable_string(p_s, phn_sys.sys_conf.enable_beep);
			break;
		default:
			phn_sys.save_chg_flga &= ~CHG_SYSTEM_CONF;
			break;
	}
	
	
}
//从字符串中返回指定顺序的数字
//如果有错误，设置err为1
int Get_str_data(char *s_data, char* separator, int num, uint8_t	*err)
{
	int 		tmp;
	int			rst = 0;
	char		*p;
	uint16_t	num_spt = 0;

	
	*err = 1;
	
	tmp = strcspn(s_data, "0123456789");
	if((tmp == 0) && (s_data[0] > '9' || s_data[0] <'0'))
		goto exit;
	p = tmp + s_data;
	if(num == 0)
	{
		*err = 0;
		rst = atoi(p);
		goto exit;
	}
	
	while(1)
	{
		tmp = strcspn(p, separator);
		if(tmp)
		{
			p += tmp + 1;
			num_spt ++;
		}
		
		if(num_spt == num)		//分隔符与序号相等说明当前的数字符合要求
		{
			*err = 0;
			rst = atoi(p);
			goto exit;
		}
		
		if(tmp == 0 || p[0] == '\0')
			goto exit;
	}
	
	exit:
	return rst;
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void Break_deal_string(char *p, int deal)
{
	
	switch(deal)
	{
		case 0:
			sprintf(p, "始点");
			break;
		case 1:
			sprintf(p, "保持");
			break;
		case 2:
			sprintf(p, "终点");
			break;
		default:
			break;
		
	}
	
}

static void Disable_string(char *p, int able)
{
	
	if(able == 0)
		sprintf(p, "禁止");
	else 
		sprintf(p, "允许");
	
}




