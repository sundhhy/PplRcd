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

#include "Modbus_app.h"
#include "channel_accumulated.h"
#include "TDD.h"
//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define 	PHN_MAJOR_VER				2
#define 	PHN_MINOR_VER				20


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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------


//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//要在存储功能初始化之后调用
void System_power_on(void)
{
	Storage			*stg = Get_storage();
	rcd_alm_pwr_t			stg_pwr = {0};
	int						num_pwr = 0;
	
	
	
	
	
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
//	else
//		phn_sys.pwr_rcd_index = num_pwr + 1;
//	if(num_pwr)
	phn_sys.pwr_rcd_index = num_pwr;
//	else
//		phn_sys.pwr_rcd_index = 0;
	//记录上电时间
	stg_pwr.flag = 1;
	stg_pwr.happen_time_s = SYS_time_sec();
	stg_pwr.disapper_time_s = 0xffffffff;
	STG_Set_file_position(STG_LOSE_PWR, STG_DRC_WRITE, phn_sys.pwr_rcd_index * sizeof(rcd_alm_pwr_t));
	
	stg->wr_stored_data(stg, STG_LOSE_PWR, &stg_pwr, sizeof(rcd_alm_pwr_t));
	
}

void System_power_off(void)
{
	int 				retry = 5;
	uint32_t		dsp_time = 0;
	Storage			*stg = Get_storage();
	
	phn_sys.sys_flag |= SYSFLAG_URGENCY;
	//掉电信息无效时，就不要存储掉电时间了
	//当擦执行了擦除掉电信息操作的时候，会出现这种情况
	if(phn_sys.pwr_rcd_index != 0xff)
	{
	
		
	
		dsp_time = SYS_time_sec();
		STG_Set_file_position(STG_LOSE_PWR, STG_DRC_WRITE, \
		phn_sys.pwr_rcd_index * sizeof(rcd_alm_pwr_t) +(int)(&((rcd_alm_pwr_t *)0)->disapper_time_s));
		
		while(stg->wr_stored_data(stg, STG_LOSE_PWR, &dsp_time, sizeof(uint32_t)) != sizeof(uint32_t))
		{
			if(retry)
				retry --;
			else
				break;
			
		}
	}

	
//	stg->wr_stored_data(stg, STG_SYS_CONF, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
	
	phn_sys.fs.fs_shutdown();
	phn_sys.sys_flag &= ~SYSFLAG_URGENCY;

}

void System_default(void)
{
	system_conf_t 	*p_sc = &phn_sys.sys_conf;
	Storage			*stg = Get_storage();
	
	memset(p_sc, 0, sizeof(system_conf_t));
	p_sc->sys_flag = 0;
	p_sc->num_chn = NUM_CHANNEL;
	p_sc->cold_end_way = 0;
	stg->wr_stored_data(stg, STG_SYS_CONF, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
	
	
	
//	p_sc->baud_idx = 0;
//	p_sc->baud_rate = arr_baud[0];
//	p_sc->disable_view_chn_status = 0;
}

void System_init(void)
{
//	struct  tm stm;
	Model 		*m;
	Storage					*stg = Get_storage();
	char			chn_name[7];
	char			i;
	int				retry = 5;

	phn_sys.major_ver = PHN_MAJOR_VER;
	phn_sys.minor_ver = PHN_MINOR_VER;
	
	phn_sys.lcd_sem_wait_ms = 0xffffffff;
	
	sys_rtc = ( UtlRtc *)Pcf8563_new();
	if(sys_rtc == NULL) while(1);

	sys_rtc->init(sys_rtc, NULL);
	sys_rtc->get(sys_rtc, &phn_sys.sys_time);
	
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

	
	CNA_Init();
	for(i = 0; i < NUM_CHANNEL; i++)
	{

		sprintf(chn_name,"chn_%d", i);
		m = ModelCreate(chn_name);
		m->init(m, &i);
		
	}
	
	MdlChn_Read_code_end_temperature();
//	System_power_off();
#endif	
	
}

extern void Ctime_Allco_time(uint16_t  all_time, uint8_t need);
int SYS_Commit(void)
{
	Storage					*stg = Get_storage();
	
	stg->wr_stored_data(stg, STG_SYS_CONF, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
	Ctime_Allco_time(phn_sys.sys_conf.record_gap_s, NUM_CHANNEL);
	
	//Modbus的串口参数也可能被修改了，所有要重新初始化MODBUS的串口
	MBA_Init_uart(phn_sys.sys_conf.baud_rate);
	return RET_OK;
}
void System_time(struct  tm *stime)
{
	
	sys_rtc->get(sys_rtc, stime);
}

uint32_t  SYS_time_sec(void)
{
	
	return Time_2_u32(&phn_sys.sys_time);
//	Model 		*m;
//	uint32_t	sec = 0;
//	m = ModelCreate("time");
//	m->getMdlData(m, TIME_U32, &sec);
	
//	return sec;
	
	
//	struct tm  t;
//	sys_rtc->get(sys_rtc, &t);
//	
//	return Time_2_u32(&t);
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
//	uint32_t sec = 0;
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














//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{





