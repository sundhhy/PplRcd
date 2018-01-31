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

#include "TDD.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define 	PHN_MAJOR_VER				0
#define 	PHN_MINOR_VER				6


const unsigned short daytab[13]={0,31,59,90,120,151,181,212,243,273,304,334,365};//非闰年月份累积天数
const unsigned short daytab1[13]={0,31,60,91,121,152,182,213,244,274,305,335,366};//闰年月份累积天数

#define    xMINUTE     (60)					/*1 ????*/
#define    xHOUR         (60*xMINUTE)			/*1 ?????*/
#define    xDAY           (24*xHOUR)			/*1 ????*/
#define    xYEAR         (365*xDAY)			/*1 ???? */

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

static UtlRtc *sys_rtc;	
//为了能够紧凑的定义一些静态变量，所以都定义在一起
//只有非4字节对齐的，需要放在这里定于
char 				*arr_p_vram[16];
uint16_t			time_smp;
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

//要在存储功能初始化之后调用
void System_power_on(void)
{
	Storage			*stg = Get_storage();
	rcd_alm_pwr_t			stg_pwr = {0};
	int								num_pwr = 0;
	
	STG_Set_file_position(STG_LOSE_PWR, STG_DRC_READ, 0);
	while(stg_pwr.flag != 0xff)
	{
		if(stg->rd_stored_data(stg, STG_LOSE_PWR, \
			&stg_pwr, sizeof(rcd_alm_pwr_t)) != sizeof(rcd_alm_pwr_t))
			{
				
				//或者已经读完了
				break;
				
				
			}
			if(stg_pwr.flag != 0xff)
				num_pwr ++;
		
	}
	
	if(num_pwr == STG_MAX_NUM_LST_PWR)
		num_pwr = 0;		
	if(num_pwr)
		phn_sys.pwr_rcd_index = num_pwr + 1;
	else
		phn_sys.pwr_rcd_index = 0;
	//记录上电时间
	STG_Set_file_position(STG_LOSE_PWR, STG_DRC_WRITE, phn_sys.pwr_rcd_index * sizeof(rcd_alm_pwr_t));
	stg_pwr.flag = 1;
	stg_pwr.happen_time_s = SYS_time_sec();
	stg->wr_stored_data(stg, STG_LOSE_PWR, &stg_pwr, sizeof(rcd_alm_pwr_t));
	
}

void System_power_off(void)
{
	int 				retry = 5;
	uint32_t		dsp_time = 0;
	Storage			*stg = Get_storage();
	
	dsp_time = SYS_time_sec();
	STG_Set_file_position(STG_LOSE_PWR, STG_DRC_WRITE, phn_sys.pwr_rcd_index * sizeof(rcd_alm_pwr_t) +(int)(&((rcd_alm_pwr_t *)0)->disapper_time_s));
	
	while(stg->wr_stored_data(stg, STG_LOSE_PWR, &dsp_time, sizeof(uint32_t)) != sizeof(uint32_t))
	{
		if(retry)
			retry --;
		else
			break;
		
	}
}

void System_default(void)
{
	system_conf_t 	*p_sc = &phn_sys.sys_conf;
	Storage			*stg = Get_storage();
	
	memset(p_sc, 0, sizeof(system_conf_t));
	p_sc->sys_flag = 0;
	p_sc->num_chn = NUM_CHANNEL;
	stg->wr_stored_data(stg, STG_SYS_CONF, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
	
	
	
//	p_sc->baud_idx = 0;
//	p_sc->baud_rate = arr_baud[0];
//	p_sc->disable_view_chn_status = 0;
}

void System_init(void)
{
	struct  tm stm;
	Model 		*m;
	Storage					*stg = Get_storage();
	char			chn_name[7];
	char			i;

	phn_sys.major_ver = PHN_MAJOR_VER;
	phn_sys.minor_ver = PHN_MINOR_VER;
	
	phn_sys.lcd_sem_wait_ms = 0xffffffff;
	
	sys_rtc = ( UtlRtc *)Pcf8563_new();
	sys_rtc->init(sys_rtc, NULL);
	sys_rtc->get(sys_rtc, &stm);
	
	//md_time要系统时间初始化之后初始化
	m = ModelCreate("time");
	m->init(m, NULL);
	
	
	w25q_init();
	FM25_init();
	EFS_init(NUM_FSH);
	
#if TDD_ON	== 0
	stg->init(stg);
	
	System_power_on();
	
	stg->rd_stored_data(stg, STG_SYS_CONF, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
	if(phn_sys.sys_conf.num_chn != NUM_CHANNEL)
		System_default();
	
	
	for(i = 0; i < NUM_CHANNEL; i++)
	{

		sprintf(chn_name,"chn_%d", i);
		m = ModelCreate(chn_name);
		m->init(m, &i);
		
	}

#endif	
	
}
void System_time(struct  tm *stime)
{
	
	sys_rtc->get(sys_rtc, stime);
}

uint32_t  SYS_time_sec(void)
{
	struct tm  t;
	sys_rtc->get(sys_rtc, &t);
	
	return Time_2_u32(&t);
}





//--------------------------------------------------------------------------------
//计算从2000年1月1号0时0分0秒到现在的秒数
//年份的值是只有两位数，否则计算不对
//----------------------------------------------------------------------------------
uint32_t    Time_2_u32(struct  tm	*tm_2_sec)
{

	unsigned char a;
	unsigned short b;
	unsigned long seconds;

	a=tm_2_sec->tm_year/4;
	seconds=(unsigned long)1461*a*xDAY;	//过去的整年秒数
	a=tm_2_sec->tm_year%4;
	if(a==0)
	{
		a = tm_2_sec->tm_mon - 1;
		b = daytab1[a];
	}
	else
	{
		b=366;
		while(--a)
		{
			b=b+365;
		}
		a = tm_2_sec->tm_mon - 1;
		b=b+daytab[a];
	}
	seconds +=xDAY*(b+tm_2_sec->tm_mday-1);//加上本月已过的天数的秒数
	seconds += xHOUR*tm_2_sec->tm_hour;				//加上本日已过的小时
	seconds += xMINUTE*tm_2_sec->tm_min;			//加上本分钟已经过去的秒数
	seconds += tm_2_sec->tm_sec;					//??????
	return seconds;

	
	
}


int  Str_time_2_tm(char *s_time, struct  tm	*time)
{
	char *p = s_time;
	struct  tm	t = {0};
	uint8_t		err = 0;
	uint16_t	i;
	t.tm_year = Get_str_data(p, "/", 0, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_mon = Get_str_data(p, "/", 1, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_mday = Get_str_data(p, "/", 2, &err);
	if(err)
		return ERR_PARAM_BAD;
	if(t.tm_mday > g_moth_day[t.tm_mon])
		return ERR_PARAM_BAD;
		
	
	i = strcspn(p, " ");
	p += i;
	
	t.tm_hour = Get_str_data(p, ":", 0, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_min = Get_str_data(p, ":", 1, &err);
	if(err)
		return ERR_PARAM_BAD;
	t.tm_sec = Get_str_data(p, ":", 2, &err);
	if(err)
		return ERR_PARAM_BAD;
	
	
	time->tm_year = t.tm_year;
	time->tm_mon = t.tm_mon;
	time->tm_mday = t.tm_mday;
	time->tm_hour = t.tm_hour;
	time->tm_min = t.tm_min;
	time->tm_sec = t.tm_sec;
	
	return RET_OK;
}

uint32_t  Str_time_2_u32(char *s_time)
{
	uint32_t sec = 0;
	struct  tm	t = {0};
	
	
	if(Str_time_2_tm(s_time, &t) != RET_OK)
		return 0xffffffff;
	return Time_2_u32(&t);	
	
}



//将秒值转换成年月日时分秒
extern int Sec_2_tm(uint32_t seconds, struct  tm *time)
{
	
	
	unsigned char a,c,i;
	unsigned short b,d;
	unsigned long x;

		
	x=(unsigned long)24*3600;
	b=(unsigned short)((seconds/x)&0xffff);
	a=(unsigned char)((b/1461)&0xff);
	a=a*4;
	b=b%1461;
	if(b<366)
	{	
		c=0;
		d=b-0;
		time->tm_year = a + c;
		i=1;
		while (d>daytab1[i]-1)
		{
			i++;
		}
		time->tm_mon = i;
		d=d-daytab1[i-1]+1;
		time->tm_mday = d;
	}
	else
	{
	  if((b>=366) && (b<731))
		{c=1;d=b-366;}
	  if((b>=731) && (b<1096))
		{c=2;d=b-731;}
	  if((b>=1096) && (b<1461))
		{c=3;d=b-1096;}
		a=a+c;
		time->tm_year = a;
		i=1;
		while (d>daytab[i]-1)
		{
			i++;
		}
		time->tm_mon = i;
		d=d-daytab[i-1]+1;
		time->tm_mday = d;
	}
	x=(unsigned long)24*3600;
	x=seconds%x;
	time->tm_hour = x / 3600;
	b=x%3600;
	time->tm_min = b / 60;
	time->tm_sec = b % 60;
	
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




