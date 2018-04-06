
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "sys_cmd.h"
#include "arithmetic/bit.h"
#include "system.h"
#include "device.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
                // thread object






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

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define NUM_RUN		2
#define NUM_TIME_TASK		8
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct {
	uint32_t		run_times;
	time_task	task;
	void		*arg;
}time_task_t;


struct {
	uint8_t		set_free_run;
	uint8_t		set_free_tts;
	uint8_t		set_clean_tts;		//要求清除的定时任务集合。

	uint8_t		set_free_idle_run;
	time_task_t	arr_tts[NUM_TIME_TASK];
	cmd_recv 	arr_cmd_run[NUM_RUN];
	cmd_recv 	cmd_idle_run;
}run_mgr;



//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static void Cmd_Thread (void const *argument);
osThreadId tid_cmd_Thread;                                          // thread id
osThreadDef (Cmd_Thread, osPriorityNormal, 1, 0);   


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int Init_Cmd_Thread (void) {

  tid_cmd_Thread = osThreadCreate (osThread(Cmd_Thread), NULL);
  if (!tid_cmd_Thread) return(-1);
  
	run_mgr.set_free_run = (1 << NUM_RUN) - 1;
	run_mgr.set_free_tts = (1 << NUM_TIME_TASK) - 1;
	run_mgr.set_clean_tts = 0;
	run_mgr.set_free_idle_run = 0xff;
  return(0);
}

int	Cmd_Rgt_recv(cmd_recv	crv)
{
	int i;
	for(i = 0; i < NUM_RUN; i++)
	{
		
		if(Check_bit(&run_mgr.set_free_run, i))
		{
			
			
			run_mgr.arr_cmd_run[i] = crv;
			Clear_bit(&run_mgr.set_free_run, i);
			return i;
		}
	}
	
	return -1;
	
}
int	Cmd_Rgt_time_task(time_task	tsk, void *arg, int time_s)
{
	
	int i;
	uint32_t cur_s = SYS_time_sec();
	for(i = 0; i < NUM_TIME_TASK; i++)
	{
		
		if(Check_bit(&run_mgr.set_free_tts, i))
		{
			
			
			run_mgr.arr_tts[i].run_times = cur_s + time_s;
			run_mgr.arr_tts[i].task = tsk;
			run_mgr.arr_tts[i].arg = arg;
			Clear_bit(&run_mgr.set_free_tts, i);
			return i;
		}
	}
	
	return -1;
	
}

void Cmd_del_time_task(int	cmd_fd)
{
	if(cmd_fd >= NUM_RUN)
		return;

	Set_bit(&run_mgr.set_clean_tts, cmd_fd);
	
}
int	Cmd_Rgt_idle_task(cmd_recv	crv)
{
	run_mgr.cmd_idle_run = crv;
	return 0;
	
}
void Cmd_del_recv(int	cmd_fd)
{
	if(cmd_fd >= NUM_RUN)
		return;
	
	run_mgr.arr_cmd_run[cmd_fd]= NULL;
	Set_bit(&run_mgr.set_free_run, cmd_fd);
	
}

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void Cmd_Thread (void const *argument) {

	short 	i;

	uint32_t cur_s ;

	
	while (1) {


		cur_s = SYS_time_sec();
		for(i = 0; i < NUM_RUN; i++)
		{
			if(run_mgr.arr_cmd_run[i])
			run_mgr.arr_cmd_run[i]();
		}

		if(run_mgr.cmd_idle_run)
			run_mgr.cmd_idle_run();

		for(i = 0; i < NUM_TIME_TASK; i++)
		{
			if(Check_bit(&run_mgr.set_clean_tts, i))
			{
				//使用set_clean_tts中转，是为了减少多线程竞争引起的后果
				Clear_bit(&run_mgr.set_clean_tts, i);
				
				run_mgr.arr_tts[i].run_times = 0xffffffff;
				Set_bit(&run_mgr.set_free_tts, i);
			}
			if(Check_bit(&run_mgr.set_free_tts, i))
				continue;
			if(cur_s < run_mgr.arr_tts[i].run_times)
				continue;
			Set_bit(&run_mgr.set_free_tts, i);
			run_mgr.arr_tts[i].task(run_mgr.arr_tts[i].arg);	
			
		}
    osThreadYield ();  		// suspend thread
  }
}


