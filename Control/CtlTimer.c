#include "CtlTimer.h"
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "sdhDef.h"
#include "system.h"
#include "ModelFactory.h"

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

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
static void Init_ctime( Controller *self, void *arg);


static void Ctime_periodic (void const *arg);

static osTimerId ctime_id;                                           // timer id
static osTimerDef (ctime, Ctime_periodic);
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
    status = osTimerStart (ctime_id, 1000);            
    if (status != osOK) {
      // Timer could not be started
			ret = ERR_OPT_FAILED;
    }
  } 
	else 
	{
		ret = ERR_OPT_FAILED;
		
	}
	
	
	
	assert(ret == RET_OK);
	cthis->time_count = 0;
	next_record = g_system.record_gap_s;
	
}

//每秒执行一次
static void Ctime_periodic (void const *arg) 
{
  // add user code here
	CtlTimer	*cthis = SUB_PTR( arg, Controller, CtlTimer);
	Model 		*p_md;
	char			chn_name[7];
	char			i;
	
	cthis->time_count ++;
	
	if(next_record)
	{
		next_record --;
		return;
	} 
	next_record = g_system.record_gap_s;
	p_md = ModelCreate("time");
	p_md->run(p_md);
	
	for(i = 0; i < NUM_CHANNEL; i++)
	{
		sprintf(chn_name,"chn_%d", i);
		p_md = ModelCreate(chn_name);
		p_md->run(p_md);
		
	}
	
}
