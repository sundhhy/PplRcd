//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#ifndef _INC_time_H_
#define _INC_time_H_
#include "stdint.h"
#include "stm32f10x_conf.h"


//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
//#define MAX_COMMA 256
#define MAX_ALARM_TOP		8			//支持的最多的闹铃数量 4个TCP连接和1个模式转换闹铃共5个
#define ALARM_CHGWORKINGMODE	0			//485从默认模式转换到工作模式的时间
#define ALARM_SENDTCPBUF		0			//发送TCP缓存中的数据的闹钟
#define ALARM_GPRSLINK(n)		(1+n)			//GPRS link
//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
struct  tm {
        int  tm_sec;    /* seconds after the minute -- [0,61] */
        int  tm_min;    /* minutes after the hour   -- [0,59] */
        int  tm_hour;   /* hours after midnight     -- [0,23] */
        int  tm_mday;   /* day of the month         -- [1,31] */
        int  tm_mon;    /* months since January     -- [0,11] */
        int  tm_year;   /* years since 1900                   */
        int  tm_wday;   /* days since Sunday        -- [0,6]  */
//        int  tm_yday;   /* days since January 1     -- [0,365]*/
//        int  tm_isdst;  /* Daylight Savings Time flag */
//        long int tm_gmtoff; /* Offset from gmt */
//        const char *tm_zone;    /* String for zone name */
};

typedef struct TIME_T
{		
		uint16_t 		period_us;
		uint16_t		count_us;
		uint32_t    	time_s;
		uint32_t		time_ms;

   
}TIME_T;
typedef void (*time_job)(void *arg);
typedef struct  
{
	void 		*next;
	void		*arg;
	uint32_t 	period_us;
	uint32_t	count_us;

	time_job 	my_job;
	
}time_task_manager;

//------------------------------------------------------------------------------
// Type definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------




void InitTimer(TIM_TypeDef *time, uint32_t us);
void regist_timejob( uint32_t period_us, time_job job, void *arg);
void clean_time_flags(void);
uint32_t get_time_s(void);
uint64_t get_time_ms(void);
void set_alarmclock_s(int alarm_id, int sec);
void set_alarmclock_ms(int alarm_id, int msec);
int Ringing(int alarm_id);
void time_test(void);
void TimeRunms( int ms);
#endif
