#include "CtlTimer.h"
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "sdhDef.h"
#include "system.h"
#include "ModelFactory.h"
#include "os/os_depend.h"
#include "utils/Storage.h"
#include "arithmetic/bit.h"
#include "channel_accumulated.h"
#ifdef NO_ASSERT
#include "basis/assert.h"
#else
#include "assert.h"
#endif
//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
//周期性的执行任务的功能
//用于定时驱动的任务
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

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define  TIME_PERIODIC_MS 	500
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------

static uint8_t	chn_smp_time[8];			//每个通道的采集时间分配
static uint32_t	las_sys_sec = 0;
//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Init_ctime( Controller *self, void *arg);


static void Ctime_periodic (void const *arg);

static osTimerId ctime_id;                                           // timer id
static osTimerDef (ctime, Ctime_periodic);
void Ctime_Allco_time(uint16_t  all_time, uint8_t need);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//



CTOR( CtlTimer)
SUPER_CTOR( Controller);
FUNCTION_SETTING( Controller.init, Init_ctime);
END_CTOR
//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{

static void Init_ctime( Controller *self, void *arg)
{
	
	int	ret = RET_OK;
	CtlTimer	*cthis = SUB_PTR( self, Controller, CtlTimer);
	osStatus status;                                              // function return status
	ctime_id = osTimerCreate (osTimer(ctime), osTimerPeriodic, self);
  if (ctime_id != NULL) {    // Periodic timer created
    // start timer with periodic 1000ms interval
    status = osTimerStart (ctime_id, TIME_PERIODIC_MS);            
    if (status != osOK) {
      // Timer could not be started
			ret = ERR_OPT_FAILED;
    }
  } 
	else 
	{
		ret = ERR_OPT_FAILED;
		
	}
	
	las_sys_sec = SYS_time_sec();
	
	
	
	assert(ret == RET_OK);
	Ctime_Allco_time(phn_sys.sys_conf.record_gap_s, NUM_CHANNEL);
	cthis->time_count = 0;
	time_smp = 0;
	
}

//每秒执行一次
static void Ctime_periodic (void const *arg) 
{
  // add user code here
	CtlTimer	*cthis = SUB_PTR( arg, Controller, CtlTimer);
	Model 		*p_md;
	Storage		*stg = Get_storage();
	uint32_t		cur_sys_sec;
	do_out_t		d;
	char			chn_name[7];
	char			i;
	
	cur_sys_sec = SYS_time_sec();
	if(cur_sys_sec == las_sys_sec)		//秒值走了才执行
		return;
	
	las_sys_sec = cur_sys_sec;
	cthis->time_count ++;
//	p_md = Create_model("time");
//	p_md->run(p_md);
//	
//	if(phn_sys.save_chg_flga & CHG_SYSTEM_CONF)
//	{
//		stg->wr_stored_data(stg, STG_SYS_CONF, &phn_sys.sys_conf, sizeof(phn_sys.sys_conf));
//		
//		phn_sys.save_chg_flga &= ~CHG_SYSTEM_CONF;
//		Ctime_Allco_time(phn_sys.sys_conf.record_gap_s, NUM_CHANNEL);
//	}
//	for(i = 0; i < NUM_CHANNEL; i++)
//	{
//		if(phn_sys.save_chg_flga & CHG_MODCHN_CONF(i))
//		{
//			
//			
//			
//			stg->wr_stored_data(stg, STG_CHN_CONF(i), NULL, 0);
//			phn_sys.save_chg_flga &= ~CHG_MODCHN_CONF(i);
//			
//		}
//		
//		
//	}
	
	
	
	
	
	
	
	if((cthis->time_count % 30) == 0)
		MdlChn_Read_code_end_temperature();

	for(i = 0; i < phn_sys.sys_conf.num_chn; i++)
	{
		if(chn_smp_time[i] != time_smp)
			continue;
		sprintf(chn_name,"chn_%d", i);
		p_md = Create_model(chn_name);
		p_md->run(p_md);
	}
	
	
	CNA_Run(1000);
	
	if(time_smp < phn_sys.sys_conf.record_gap_s)
	{
		time_smp ++;
	} 
	else
		time_smp = 0;
	
	for(i = 0; i < MAX_TOUCHSPOT; i++)
	{
		d.do_chn = i;
		if(phn_sys.DO_err & (1 << i))
		{
			
			d.val = 1;
			p_md->setMdlData(p_md, DO_output, &d);
		}
		else if(phn_sys.DO_val & (1 << i))		//清除报警
		{
			d.val = 0;
			p_md->setMdlData(p_md, DO_output, &d);
		}
		
	}
	phn_sys.DO_err = 0;
	
	
}

//todo:180121 目前的实现方式是只考虑6通道的情况下
//把每个通道的采集时间打散，让每次的处理负荷尽可能的平均
void Ctime_Allco_time(uint16_t  all_time, uint8_t need)
{
	
	int i = 0; 
	
	switch(all_time)
	{
		case 0:
		case 1:
			for(i = 0; i < 8; i++)
				chn_smp_time[i] = 0;
			break;
		case 2:
			chn_smp_time[0] = 0;
			chn_smp_time[1] = 0;
			chn_smp_time[2] = 0;
			chn_smp_time[3] = 1;
			chn_smp_time[4] = 1;
			chn_smp_time[5] = 1;
			break;
		case 3:
		case 4:
		case 5:
			chn_smp_time[0] = 0;
			chn_smp_time[1] = 0;
			chn_smp_time[2] = 1;
			chn_smp_time[3] = 1;
			chn_smp_time[4] = 2;
			chn_smp_time[5] = 2;
			break;
		default:
			chn_smp_time[0] = 0;
			chn_smp_time[1] = 1;
			chn_smp_time[2] = 2;
			chn_smp_time[3] = 3;
			chn_smp_time[4] = 4;
			chn_smp_time[5] = 5;
			break;
			
		
		
		
	}
	
	
}
