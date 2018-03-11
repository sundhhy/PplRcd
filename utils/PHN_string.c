//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//
#include <string.h>
#include <stdio.h>
#include <math.h>


#include "PHN_string.h"
#include "system.h"
#include "Model.h"
#include "Model_channel.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

static const int	arr_baud[7] = {1200, 4800, 9600, 19200, 38400, 57200, 115200};

static const char *su_Nm3_h = "Nm3/h";
static const char *su_m3_h = "m3/h";
static const char *su_m3_min = "m3/min";
static const char *su_m3_s = "m3/s";
static const char *su_L_h = "L/h";
static const char *su_L_min = "L/min";
static const char *su_L_s = "L/s";
static const char *su_kg_h = "kg/h";
static const char *su_kg_min = "kg/min";
static const char *su_kg_s = "kg/s";
static const char *su_oC = "℃";
static const char *su_Pa = "Pa";
static const char *su_MPa = "MPa";
static const char *su_kgf_cm2 = "kgf/cm2";
static const char *su_Bar = "Bar";
static const char *su_mmH2O = "mmH2O";
static const char *su_mmHg = "mmHg";
static const char *su_percent = "%";			//%
static const char *su_ppm = "ppm";
static const char *su_pH = "pH";
static const char *su_r_min = "r/min";
static const char *su_Hz = "Hz";
static const char *su_kHz = "kHz";
static const char *su_mA = "mA";
static const char *su_A = "A";
static const char *su_kA = "kA";
static const char *su_mV = "mV";
static const char *su_V = "V";
static const char *su_kV = "kV";
static const char *su_W = "W";
static const char *su_kW = "kW";
static const char *su_MW = "MW";
static const char *su_Var = "Var";
static const char *su_kVar = "kVar";
static const char *su_MVar = "MVar";
static const char *su_J = "J";
static const char *su_kJ = "kJ";
static const char *su_uS_cm = "uS/cm";
static const char *su_J_kgoC = "J/kg℃";
static const char *su_kWh = "kWh";
static const char *su_ug_L = "ug/L";

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
void Print_sys_param(void *p_data, char	*p_s, int len, int aux)
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
			{
				p_u8 = (uint8_t *)p_data;
				sprintf(p_s, "%2d", *p_u8);
			}
			else 
			{
				if(phn_sys.sys_conf.cold_end_way)
				{

					sprintf(p_s, "%2d", phn_sys.sys_conf.CJC);
				}
				else
				{
					sprintf(p_s, "%2d", phn_sys.code_end_temperature);
				}
				
			}
			
			
			break;
			
		case es_cold_end_way:
			if(p_data)
				p_u8 = (uint8_t *)p_data;
			else 
				p_u8 = &phn_sys.sys_conf.cold_end_way;
			if(*p_u8 ==0) {
				
				sprintf(p_s, "外部");
			} else {
				sprintf(p_s, "设定");
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

void Str_set_sys_param(char	*p_s, int aux, int op, int val)
{
	switch(aux)
	{
		case es_rcd_t_s:
			phn_sys.sys_conf.record_gap_s = Operate_in_tange(phn_sys.sys_conf.record_gap_s, op, val, 0, 240);
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
		case es_cold_end_way:
			phn_sys.sys_conf.cold_end_way = Operate_in_tange(phn_sys.sys_conf.cold_end_way, op, val, 0, 1);
			Print_sys_param(NULL, p_s, 0xff, es_cold_end_way);
			break;
			
		case es_CJC:
			phn_sys.sys_conf.CJC = Operate_in_tange(phn_sys.sys_conf.CJC, op, val, 0, 99);
			Print_sys_param(NULL, p_s, 0xff, es_CJC);
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

//"** ** **"
void Str_set_password(char	*p_s_psd)
{
	short	i, data;
	
	for(i = 0; i < 3; i++) {
		data = atoi(p_s_psd);
	
		phn_sys.sys_conf.password[i] = data;
		
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


// model.c

void Print_float(int data, short int_len, short prec, char *str)
{
	int		d1, d2;
	short		sign = 1;
	char		i;		//用于对齐
	if(data < 0)
	{
		sign = -1;
		data *= -1;		//转成正数才好显示
		
	}
	
	if(prec == 1) {
		d1 = data/10;
		d2 = data%10;
		
		//显示数字的符号时，把符号作为整体的符号来显示
		//去除每个部分的符号，
//		d1 = d1 * sign;
//		d2 = d2 * sign;
		
		
	}
	else if(prec == 2)  {
		d1 = data/100;
		d2 = data%100;
		
		//显示数字的符号时，把符号作为整体的符号来显示
		//去除每个部分的符号，
//		d1 = d1 * sign;
//		d2 = d2 * sign;
//		if(sign > 0)
//			sprintf(str, "%d.%02d", d1, d2);
//		else 
//			sprintf(str, "-%d.%02d", d1, d2);
		
	}
//	else
//	{
//			d1 = data;
//		
//	}
	
	if(sign > 0)
	{
//		if(int_len == 4)
//			sprintf(str, "%4d.%d", d1, d2);
//		else if(int_len == 4)
//			sprintf(str, "%2d.%d", d1, d2);
//		else
		if(prec)
			sprintf(str, "%d.%d", d1, d2);
		else
			sprintf(str, "%d", data);
	}
	else 
	{
//		if(int_len == 4)
//			sprintf(str, "-%4d.%d", d1, d2);
//		else if(int_len == 4)
//			sprintf(str, "-%2d.%d", d1, d2);
//		else
		if(prec)
			sprintf(str, "-%d.%d", d1, d2);
		else
			sprintf(str, "-%d", data);
		
		
	}
	
	for(i = strlen(str); i < int_len; i ++)
	{
		
		str[i] = ' ';
		
	}
	
}

void Print_frefix_float(int data, int prec, char *prefix, char *str)
{
	
	if(prec == 1)
		sprintf(str, "%s%d.%d", prefix, data/10, data%10);
	else if(prec == 2) 
		sprintf(str, "%s%d.%02d", prefix, data/100, data%100);
}

void Print_unit( char unit, char *buf, int len)
{
	switch( unit)
	{
		case eu_Nm3_h:
			strncpy( buf,su_Nm3_h, len);
			break;
		case eu_m3_h:
			strncpy( buf,su_m3_h, len);
			break;
		case eu_m3_min:
			strncpy( buf,su_m3_min, len);
			break;
		case eu_m3_s:
			strncpy( buf,su_m3_s, len);
			break;
		case eu_L_h:
			strncpy( buf,su_L_h, len);
			break;
		case eu_L_min:
			strncpy( buf,su_L_min, len);
			break;	
		case eu_L_s:
			strncpy( buf,su_L_s, len);
			break;
		case eu_kg_h:
			strncpy( buf,su_kg_h, len);
			break;
		case eu_kg_min:
			strncpy( buf,su_kg_min, len);
			break;
		case eu_kg_s:
			strncpy( buf,su_kg_s, len);
			break;
		case eu_oC:
			strncpy( buf,su_oC, len);
			break;
		case eu_Pa:
			strncpy( buf,su_Pa, len);
			break;	
		case eu_MPa:
			strncpy( buf,su_MPa, len);
			break;
		case eu_kgf_cm2:
			strncpy( buf,su_kgf_cm2, len);
			break;
		case eu_Bar:
			strncpy( buf,su_Bar, len);
			break;
		case eu_mmH2O:
			strncpy( buf,su_mmH2O, len);
			break;
		case eu_mmHg:
			strncpy( buf,su_mmHg, len);
			break;
		case eu_percent:
			strncpy( buf,su_percent, len);
			break;	
		case eu_ppm:
			strncpy( buf,su_ppm, len);
			break;
		case eu_pH:
			strncpy( buf,su_pH, len);
			break;
		case eu_r_min:
			strncpy( buf,su_r_min, len);
			break;
		case eu_Hz:
			strncpy( buf, su_Hz, len);
			break;
		case eu_kHz:
			strncpy( buf,su_kHz, len);
			break;
		case eu_mA:
			strncpy( buf,su_mA, len);
		case eu_A:
			strncpy( buf,su_A, len);
			break;
		case eu_kA:
			strncpy( buf,su_kA, len);
			break;
		case eu_mV:
			strncpy( buf,su_mV, len);
			break;
		case eu_V:
			strncpy( buf,su_V, len);
			break;
		case eu_kV:
			strncpy( buf,su_kV, len);
			break;
		case eu_W:
			strncpy( buf,su_W, len);
			break;	
		case eu_kW:
			strncpy( buf,su_kW, len);
			break;
		case eu_MW:
			strncpy( buf,su_MW, len);
			break;
		case eu_Var:
			strncpy( buf,su_Var, len);
			break;
		case eu_kVar:
			strncpy( buf,su_kVar, len);
			break;
		case eu_MVar:
			strncpy( buf,su_MVar, len);
			break;	
		case eu_J:
			strncpy( buf,su_J, len);
			break;
		case eu_kJ:
			strncpy( buf,su_kJ, len);
			break;
		case eu_uS_cm:
			strncpy( buf,su_uS_cm, len);
			break;
		case eu_J_kgoC:
			strncpy( buf,su_J_kgoC, len);
			break;
		case eu_kWh:
			strncpy( buf,su_kWh, len);
			break;
		case eu_ug_L:
			strncpy( buf,su_ug_L, len);			
			
			
			
			break;		
		default:
			strncpy( buf, "m3/h", len);
			break;
		
	}
	
}

int Mdl_string_to_unit( char *s_unit, int len)
{
	char *p;
	int		u = -1;
	
	p = strstr(s_unit, su_Nm3_h);
	if(p) {
		u = eu_Nm3_h;
		goto exit;
	}
	p = strstr(s_unit, su_m3_h);
	if(p) {
		u = eu_m3_h;
		goto exit;
	}
	p = strstr(s_unit, su_m3_min);
	if(p) {
		u = eu_m3_min;
		goto exit;
	}
	p = strstr(s_unit, su_m3_s);
	if(p) {
		u = eu_m3_s;
		goto exit;
	}		
	p = strstr(s_unit, su_L_h);
	if(p) {
		u = eu_L_h;
		goto exit;
	}
	p = strstr(s_unit, su_L_min);
	if(p) {
		u = eu_L_min;
		goto exit;
	}
	p = strstr(s_unit, su_L_s);
	if(p) {
		u = eu_L_s;
		goto exit;
	}
	p = strstr(s_unit, su_kg_h);
	if(p) {
		u = eu_kg_h;
		goto exit;
	}		
	p = strstr(s_unit, su_kg_min);
	if(p) {
		u = eu_kg_min;
		goto exit;
	}
	p = strstr(s_unit, su_kg_s);
	if(p) {
		u = eu_kg_s;
		goto exit;
	}
	p = strstr(s_unit, su_oC);
	if(p) {
		u = eu_oC;
		goto exit;
	}
	p = strstr(s_unit, su_Pa);
	if(p) {
		u = eu_Pa;
		goto exit;
	}		
	p = strstr(s_unit, su_MPa);
	if(p) {
		u = eu_MPa;
		goto exit;
	}
	p = strstr(s_unit, su_kgf_cm2);
	if(p) {
		u = eu_kgf_cm2;
		goto exit;
	}
	p = strstr(s_unit, su_Bar);
	if(p) {
		u = eu_Bar;
		goto exit;
	}
	p = strstr(s_unit, su_mmH2O);
	if(p) {
		u = eu_mmH2O;
		goto exit;
	}	
		p = strstr(s_unit, su_mmHg);
	if(p) {
		u = eu_mmHg;
		goto exit;
	}
	p = strstr(s_unit, su_percent);
	if(p) {
		u = eu_percent;
		goto exit;
	}
	p = strstr(s_unit, su_ppm);
	if(p) {
		u = eu_ppm;
		goto exit;
	}
	p = strstr(s_unit, su_pH);
	if(p) {
		u = eu_pH;
		goto exit;
	}	
	
	
	
	p = strstr(s_unit, su_r_min);
	if(p) {
		u = eu_r_min;
		goto exit;
	}
	p = strstr(s_unit, su_Hz);
	if(p) {
		u = eu_Hz;
		goto exit;
	}
	p = strstr(s_unit, su_mA);
	if(p) {
		u = eu_mA;
		goto exit;
	}
	p = strstr(s_unit, su_A);
	if(p) {
		u = eu_A;
		goto exit;
	}		
	p = strstr(s_unit, su_kA);
	if(p) {
		u = eu_kA;
		goto exit;
	}
	p = strstr(s_unit, su_mV);
	if(p) {
		u = eu_mV;
		goto exit;
	}
	p = strstr(s_unit, su_V);
	if(p) {
		u = eu_V;
		goto exit;
	}
	p = strstr(s_unit, su_kV);
	if(p) {
		u = eu_kV;
		goto exit;
	}		
	p = strstr(s_unit, su_W);
	if(p) {
		u = eu_W;
		goto exit;
	}
	p = strstr(s_unit, su_kW);
	if(p) {
		u = eu_kW;
		goto exit;
	}
	p = strstr(s_unit, su_MW);
	if(p) {
		u = eu_MW;
		goto exit;
	}
	p = strstr(s_unit, su_Var);
	if(p) {
		u = eu_Var;
		goto exit;
	}		
	p = strstr(s_unit, su_kVar);
	if(p) {
		u = eu_kVar;
		goto exit;
	}
	p = strstr(s_unit, su_MVar);
	if(p) {
		u = eu_MVar;
		goto exit;
	}
	p = strstr(s_unit, su_J);
	if(p) {
		u = eu_J;
		goto exit;
	}
	p = strstr(s_unit, su_kJ);
	if(p) {
		u = eu_kJ;
		goto exit;
	}	
		p = strstr(s_unit, su_uS_cm);
	if(p) {
		u = eu_uS_cm;
		goto exit;
	}
	p = strstr(s_unit, su_J_kgoC);
	if(p) {
		u = eu_J_kgoC;
		goto exit;
	}
	p = strstr(s_unit, su_kWh);
	if(p) {
		u = eu_kWh;
		goto exit;
	}
	p = strstr(s_unit, su_ug_L);
	if(p) {
		u = eu_ug_L;
		goto exit;
	}	
	
	exit:
	return u;
	
}
//-------model_channel.c
void Print_touch_spot(int spot, char *str)
{
	
	if(spot < MAX_TOUCHSPOT && spot >= 0)
		sprintf(str, "%d", spot);
	else
		sprintf(str, "无");
}
void Print_singnaltype(int sgt, char *str)
{
	switch(sgt)
	{
		case AI_0_5_V:
			sprintf(str, "0~5V");
			break;
		case AI_0_10_mA:
			sprintf(str, "0~10mA");
			break;		
		case AI_1_5_V:
			sprintf(str, "1~5V");
			break;
		case AI_4_20_mA:
			sprintf(str, "4~20mA");
			break;	
		case AI_0_20_mV:
			sprintf(str, "0~20mV");
			break;
		case AI_0_100_mV:
			sprintf(str, "0~100mV");
			break;	
		case AI_Pt100:
			sprintf(str, "Pt100");
			break;
		case AI_Cu50:
			sprintf(str, "Cu50");
			break;
		case AI_B:
			sprintf(str, "B");
			break;		
		case AI_E:
			sprintf(str, "E");
			break;
		case AI_J:
			sprintf(str, "J");
			break;		
		case AI_K:
			sprintf(str, "K");
			break;
		case AI_S:
			sprintf(str, "S");
			break;	
		case AI_T:
			sprintf(str, "T");
			break;
//		case AI_0_400_ohm:
//			sprintf(str, "0-400Ω");
//			break;		
//		case PI_0_30_kHz:
//			sprintf(str, "PI");
//			break;
//		case DI_8_30_V:
//			sprintf(str, "DI 8~30V");
//			break;		
//		case DI_0_5_V:
//			sprintf(str, "DI 0~5V");
//			break;	
//		case AO_4_20_mA:
//			sprintf(str, "AO");
//			break;			
	}	
}

// ----------------  mdl_time.c
//"YY/MM/DD HH:MM:SS"
//与MdlTime_text_iteartor紧密耦合
void MdlTime_text_modify(char	*p_time_text, int idx, int op)
{
	switch(idx) {
		case 0:		//年
		case 1:
			Str_Calculations(p_time_text + idx, 1, op, 1, 0, 9);
			break;
		
		//月
		case 3:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 1);
			break;
		case 4:
			if(p_time_text[3] == '1')
				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 2);
			else 
				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
			break;
			//日
		case 6:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 3);
			break;
		case 7:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
			break;
		//时
		case 9:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 2);
			break;
		case 10:
			
			if(p_time_text[9] == '2')
				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 4);
			else 
				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
			break;
		//分
		case 12:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 5);
			break;
		case 13:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
			break;
		//秒
		case 15:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 5);
			break;
		case 16:
			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
			break;
		default:
			break;
	}
	
	//年份是4位数
//	switch(idx) {
//		case 0:		//年
//		case 1:
//			break;
//		case 2:
//		case 3:
//			Str_Calculations(p_time_text + idx, 1, op, 1, 0, 9);
//			break;
//		
//		//月
//		case 5:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 1);
//			break;
//		case 6:
//			if(p_time_text[3] == '1')
//				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 2);
//			else 
//				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
//			break;
//			//日
//		case 8:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 3);
//			break;
//		case 9:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
//			break;
//		//时
//		case 11:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 2);
//			break;
//		case 12:
//			
//			if(p_time_text[9] == '2')
//				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 4);
//			else 
//				Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
////			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
//			break;
//		//分
//		case 14:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 5);
//			break;
//		case 15:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
//			break;
//		//秒
//		case 17:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 5);
//			break;
//		case 18:
//			Str_Calculations(p_time_text + idx, 1,  op, 1, 0, 9);
//			break;
//		default:
//			break;
//	}
	
	
}





//对时间显示的字符上移动
//返回新的位置
//与MdlTime_text_modify函数紧密耦合
int MdlTime_text_iteartor(char	*p_time_text, int idx, int director)
{
	int	new_idx = 0;
	if(director == 0) {
		//左移
		if(idx == 0)
			new_idx = 16;
		else
			new_idx = idx - 1;
		//跳过非数字字符
		while(p_time_text[new_idx] > '9' || p_time_text[new_idx] < '0')
			new_idx --;
		
		
	} else {
		//右移
		if(idx >= 16)
			new_idx = 0;
		else
			new_idx = idx + 1;
		//跳过非数字字符
		while(p_time_text[new_idx] > '9' || p_time_text[new_idx] < '0')
			new_idx ++;
	
		
	}
	
	return new_idx;
	
}





//HMI_comm.c
//对数字字符串运算
//不会对字符串中的值进行合法判断
//暂时支持最大999
void Str_Calculations(char *p_str, int len, int op, int val, int rangel, int rangeh)
{
	
	int dig = 0;
	int i;
	short w[5] = {1, 10, 100, 1000, 10000};	
	char	buf[6] = {0};

	if(len > 6)
		len = 6;
	for(i = 0; i < len; i ++) {
		
		buf[i] = p_str[i];
		
	}
	
	dig = atoi(buf);
	for(i = 0; i < 6; i ++) {
		
		buf[i] = 0;
		
	}	
	
	//未指定范围
	if(rangeh == 0 && rangel == 0) {
		rangel = 0;
		rangeh = w[len] - 1;
		
	}
	
	if(op == OP_ADD) {
		if(dig < rangeh)
			dig += val;
		else 
			dig = rangel;
		
	} else if(op == OP_SUB) {
		
		if(dig > rangel)
			dig -= val;
		else 
			dig = rangeh;
	}
	else if(op == OP_MUX) {
		
		if(dig < rangeh)
			dig *= val;
		else 
			dig = rangel;
	}
	else if(op == OP_DIV) {
		if(val == 0)
			return;
		if(dig > rangel)
			dig /= val;
		else 
			dig = rangeh;
	}
	switch(len)
	{
		case 2:
			snprintf(buf, 6, "%2d", dig);
			break;
		case 3:
			snprintf(buf, 6, "%3d", dig);
			break;
		case 4:
			snprintf(buf, 6, "%4d", dig);
			break;
		default:
			snprintf(buf, 6, "%d", dig);
			break;
		
		
	}
	
	
	for(i = 0; i < len; i ++) {
		p_str[i] = buf[i];
			
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


