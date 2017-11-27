//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sdhDef.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//为了能够紧凑的定义一些静态变量，所以都定义在一起
//只有非4字节对齐的，需要放在这里定于
char 			*arr_p_vram[16];
system_conf_t	g_system;

uint16_t		next_record;
short 			cmd_count = 0;
char				g_setting_chn = 0;
char				g_system_none[3];
//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

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



void System_default(system_conf_t *arg)
{
	system_conf_t *p_sc = &g_system;
	memset(p_sc, 0, sizeof(system_conf_t));
//	p_sc->baud_idx = 0;
//	p_sc->baud_rate = arr_baud[0];
//	p_sc->disable_view_chn_status = 0;
}

void System_init(void)
{
	
	System_default(NULL);
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
	
		if( g_system.password[i] != data) {
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
	
		g_system.password[i] = data;
		
		p_s_psd += 3;
	}
	
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
				p_u8 = g_system.password;
			
			sprintf(p_s, "%02d %02d %02d", p_u8[0], p_u8[1], p_u8[2]);
			break;
		
		case es_brk_cpl:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.break_couple;
			
			Break_deal_string(p_s, *p_u8);
			break;
		case es_brk_rss:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.break_resistor;
			
			Break_deal_string(p_s, *p_u8);
			break;
		case es_baud:
			
			break;
		case es_cmn_md:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.communication_mode;
			
			if(*p_u8 == 0)
				sprintf(p_s, "通讯");
			else 
				sprintf(p_s, "打印");
			break;
		case es_mdfy_prm:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.disable_modify_adjust_paramter;
			
			Disable_string(p_s, *p_u8);
			break;
			
		case es_CJC:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.CJC;
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
				p_u8 = &g_system.disable_view_chn_status;
			
			Disable_string(p_s, *p_u8);
			break;
		case es_beep:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &g_system.enable_beep;
			
			Disable_string(p_s, *p_u8);
			break;
	}
	
	
}



void System_modify_string(char	*p_s, int aux, int op, int val)
{
	switch(aux)
	{
		case es_rcd_t_s:
			g_system.record_gap_s = Operate_in_tange(g_system.record_gap_s, op, val, 0, 99);
			next_record = g_system.record_gap_s;
			sprintf(p_s, "%d", g_system.record_gap_s);
			break;
		case es_brk_cpl:
			
			g_system.break_couple = Operate_in_tange(g_system.break_couple, op, val, 0, 2);
			Break_deal_string(p_s, g_system.break_couple);
			break;
		case es_brk_rss:
			g_system.break_resistor = Operate_in_tange(g_system.break_resistor, op, val, 0, 2);
			Break_deal_string(p_s, g_system.break_resistor);
			break;
		case es_baud:
			g_system.baud_idx = Operate_in_tange(g_system.baud_idx, op, val, 0, 6);
			g_system.baud_rate = arr_baud[g_system.baud_idx];
			sprintf(p_s, "%d", g_system.baud_rate);
			break;
		case es_id:
			g_system.id = Operate_in_tange(g_system.id, op, val, 1, 63);
			sprintf(p_s, "%d", g_system.id);
			break;
		case es_cmn_md:
			g_system.communication_mode = Operate_in_tange(g_system.communication_mode, op, val, 0, 1);
			if(g_system.communication_mode == 0)
				sprintf(p_s, "通讯");
			else 
				sprintf(p_s, "打印");
			break;
		case es_mdfy_prm:
		
			g_system.disable_modify_adjust_paramter = Operate_in_tange(g_system.disable_modify_adjust_paramter, op, val, 0, 1);
			Disable_string(p_s, g_system.disable_modify_adjust_paramter );
			break;
			
		case es_CJC:
			g_system.CJC = Operate_in_tange(g_system.CJC, op, val, 0, 100);
			System_to_string(NULL, p_s, 0xff, es_CJC);
//			if(g_system.CJC < 100) {
//				
//				sprintf(p_s, "设定 %d", g_system.CJC);
//			} else {
//				sprintf(p_s, "外部    ");
//			}
			break;
			
			
		case es_vcs:
			g_system.disable_view_chn_status = Operate_in_tange(g_system.disable_view_chn_status, op, val, 0, 1);
			Disable_string(p_s, g_system.disable_view_chn_status);
			break;
		case es_beep:
			g_system.enable_beep = Operate_in_tange(g_system.enable_beep, op, val, 0, 1);
			Disable_string(p_s, g_system.enable_beep);
			break;
	}
	
	
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




