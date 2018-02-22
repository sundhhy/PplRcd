
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include "sys_cmd.h"
#include "arithmetic/bit.h"

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
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
struct {
	uint8_t		set_free_run;
	uint8_t		set_free_idle_run;
	uint8_t		none[2];
	cmd_run_t 	arr_cmd_run[NUM_RUN];
	cmd_run_t 	cmd_idle_run;
}run_mgr;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static void Cmd_Thread (void const *argument);
osThreadId tid_cmd_Thread;                                          // thread id
osThreadDef (Cmd_Thread, osPriorityBelowNormal, 1, 0);   


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------

int	Cmd_Rgt_recv(cmd_recv	crv, void *arg)
{
	int i;
	for(i = 0; i < NUM_RUN; i++)
	{
		
		if(Check_bit(&run_mgr.set_free_run, i))
		{
			
			
			run_mgr.arr_cmd_run[i].func = crv;
			run_mgr.arr_cmd_run[i].arg = arg;
			Clear_bit(&run_mgr.set_free_run, i);
			return i;
		}
	}
	
	return -1;
	
}
int	Cmd_Rgt_idle_task(cmd_recv	crv, void *arg)
{
	run_mgr.cmd_idle_run.func = crv;
	run_mgr.cmd_idle_run.arg = arg;
	return 0;
	
}
void Cmd_del_recv(int	cmd_fd)
{
	if(cmd_fd >= NUM_RUN)
		return;
	
	run_mgr.arr_cmd_run[cmd_fd].func= NULL;
	Set_bit(&run_mgr.set_free_run, cmd_fd);
	
}

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//
int Init_Cmd_Thread (void) {

  tid_cmd_Thread = osThreadCreate (osThread(Cmd_Thread), NULL);
  if (!tid_cmd_Thread) return(-1);
  
	run_mgr.set_free_run = (1 << NUM_RUN) - 1;
	run_mgr.set_free_idle_run = 0xff;
  return(0);
}



//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{
static void Cmd_Thread (void const *argument) {

	int i;
  while (1) {
    ; // Insert thread code here...
	  for(i = 0; i < NUM_RUN; i++)
	  {
		if(run_mgr.arr_cmd_run[i].func)
			run_mgr.arr_cmd_run[i].func(run_mgr.arr_cmd_run[i].arg);
		}
	  
		 if(run_mgr.cmd_idle_run.func)
			run_mgr.cmd_idle_run.func(run_mgr.cmd_idle_run.arg);
    osThreadYield ();  		// suspend thread
  }
}


