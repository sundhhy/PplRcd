/*
 * os_depend.c
 *
 *  Created on: 2017-1-13
 *      Author: Administrator
 */


//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//
#include "os/os_depend.h"
#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <assert.h>
#include <stdio.h>
#include "basis/sdhError.h"
#include "basis/macros.h"
#include "arithmetic/bit.h"

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define MAX_NUM_SEM			16
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
struct  {
	uint8_t	set_used_sems[2];
	uint16_t	none;
	
}rtx_sem_mgr;
//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
osSemaphoreId sid_Sem0;                             
osSemaphoreDef(rtxSemaphore0); 

osSemaphoreId sid_Sem1;                             
osSemaphoreDef(rtxSemaphore1);                              

osSemaphoreId sid_Sem2;                             
osSemaphoreDef(rtxSemaphore2); 
  
osSemaphoreId sid_Sem3;                             
osSemaphoreDef(rtxSemaphore3); 

osSemaphoreId sid_Sem4;                             
osSemaphoreDef(rtxSemaphore4); 

osSemaphoreId sid_Sem5;                             
osSemaphoreDef(rtxSemaphore5); 

osSemaphoreId sid_Sem6;                             
osSemaphoreDef(rtxSemaphore6);


osSemaphoreId sid_Sem7;                             
osSemaphoreDef(rtxSemaphore7); 
#if MAX_NUM_SEM == 16
osSemaphoreId sid_Sem8;                             
osSemaphoreDef(rtxSemaphore8); 

osSemaphoreId sid_Sem9;                             
osSemaphoreDef(rtxSemaphore9); 

osSemaphoreId sid_Sem10;                             
osSemaphoreDef(rtxSemaphore10);

osSemaphoreId sid_Sem11;                             
osSemaphoreDef(rtxSemaphore11);

osSemaphoreId sid_Sem12;                             
osSemaphoreDef(rtxSemaphore12);

osSemaphoreId sid_Sem13;                             
osSemaphoreDef(rtxSemaphore13);

osSemaphoreId sid_Sem14;                             
osSemaphoreDef(rtxSemaphore14);

osSemaphoreId sid_Sem15;                             
osSemaphoreDef(rtxSemaphore15);
#endif


//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------



//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//=========================================================================//
//                                                                         //
//          P R I V A T E   D E F I N I T I O N S                          //
//                                                                         //
//=========================================================================//
/// \name Private Functions
/// \{








void delay_s(int sec)
{
	;
}

void delay_ms(int ms)
{
	osDelay(ms);
}

void delay_ns( unsigned long nsec )
{
	;
}

//不能在中断中运行
int get_currenttime(os_time_t *ref_time)
{
	*ref_time = osKernelSysTick(); 
	return RET_OK;

}

void PrintTime(os_time_t *time)
{
	;
}

int cal_timediff_ms( os_time_t *ref_time)
{

	uint32_t tick;
	uint32_t tick_1ms; 
	tick_1ms = osKernelSysTickMicroSec(1);
	tick = osKernelSysTick(); 
	if( tick > *ref_time)
		tick -= *ref_time;
	else
	{
		tick = tick + 0xffffffff - *ref_time;
		
	}
	return tick/tick_1ms;

}

int DiffTimes_ms( os_time_t *p_start, os_time_t *P_end)
{

	uint32_t tick_1ms = osKernelSysTickMicroSec(1); 
	
	if( *P_end > *p_start)
		*P_end -= *p_start;
	else
	{
		*P_end = *P_end + 0xffffffff - *p_start;
		
	}
	return *P_end/tick_1ms;

}

int Alloc_sem(void)
{
	int i = 0;
	for(i = 0; i < MAX_NUM_SEM; i++)
	{
		if(Check_bit(rtx_sem_mgr.set_used_sems, i) == 0)
		{
			Set_bit(rtx_sem_mgr.set_used_sems, i);
			return i;
		}
		
	}
	
	return -1;
	
}

int Sem_init(sem_t *sem)
{
	sem_t s = *sem;
	int ret = RET_OK;
	switch( s)
	{
		case 0:
			sid_Sem0 = osSemaphoreCreate (osSemaphore(rtxSemaphore0), 1);
			if (!sid_Sem0) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem0, 0 );
			break;
		case 1:
			sid_Sem1 = osSemaphoreCreate (osSemaphore(rtxSemaphore1), 1);
			if (!sid_Sem1) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem1, 0 );
			break;
		case 2:
			sid_Sem2 = osSemaphoreCreate (osSemaphore(rtxSemaphore2), 1);
			if (!sid_Sem2) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem2, 0 );
			break;
		case 3:
			sid_Sem3 = osSemaphoreCreate (osSemaphore(rtxSemaphore3), 1);
			if (!sid_Sem3) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem3, 0 );
			break;
		case 4:
			sid_Sem4 = osSemaphoreCreate (osSemaphore(rtxSemaphore4), 1);
			if (!sid_Sem4) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem4, 0 );
			break;
		case 5:
			sid_Sem5 = osSemaphoreCreate (osSemaphore(rtxSemaphore5), 1);
			if (!sid_Sem5) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem5, 0 );
			break;
		case 6:
			sid_Sem6 = osSemaphoreCreate(osSemaphore(rtxSemaphore6), 1);
			if (!sid_Sem6) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem6, 0 );
			break;
		case 7:
			sid_Sem7 = osSemaphoreCreate (osSemaphore(rtxSemaphore7), 1);
			if (!sid_Sem7) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem7, 0 );
			break;
			#if MAX_NUM_SEM == 16
		case 8:
			sid_Sem8 = osSemaphoreCreate (osSemaphore(rtxSemaphore8), 1);
			if (!sid_Sem8) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem8, 0 );
			break;
		case 9:
			sid_Sem9 = osSemaphoreCreate (osSemaphore(rtxSemaphore9), 1);
			if (!sid_Sem9) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem9, 0 );
			break;
		case 10:
			sid_Sem10 = osSemaphoreCreate(osSemaphore(rtxSemaphore10), 1);
			if (!sid_Sem10) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem10, 0 );
			break;
		case 11:
			sid_Sem11 = osSemaphoreCreate(osSemaphore(rtxSemaphore11), 1);
			if (!sid_Sem11) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem11, 0 );
			break;
		case 12:
			sid_Sem12 = osSemaphoreCreate(osSemaphore(rtxSemaphore12), 1);
			if (!sid_Sem12) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem12, 0 );
			break;
		case 13:
			sid_Sem13 = osSemaphoreCreate(osSemaphore(rtxSemaphore13), 1);
			if (!sid_Sem13) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem13, 0 );
			break;
		case 14:
			sid_Sem14 = osSemaphoreCreate(osSemaphore(rtxSemaphore14), 1);
			if (!sid_Sem14) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem14, 0 );
			break;
		case 15:
			sid_Sem15 = osSemaphoreCreate(osSemaphore(rtxSemaphore15), 1);
			if (!sid_Sem15) {
				ret = ERR_OSRSU_UNAVAILABLE;
			}
			osSemaphoreWait( sid_Sem15, 0 );
			break;
			#endif
		default:
			ret = ERR_BAD_PARAMETER;
			break;
		
	}
	return ret;
	
	
}

int Sem_wait(sem_t *sem, int ms)
{
	
	sem_t s = *sem;
	switch( s)
	{
		case 0:
			return osSemaphoreWait( sid_Sem0, ms );
		case 1:
			return osSemaphoreWait( sid_Sem1, ms );
		case 2:
			return osSemaphoreWait( sid_Sem2, ms );
		case 3:
			return osSemaphoreWait( sid_Sem3, ms );
		case 4:
			return osSemaphoreWait( sid_Sem4, ms );
		case 5:
			return osSemaphoreWait( sid_Sem5, ms );
		case 6:
			return osSemaphoreWait( sid_Sem6, ms );
		case 7:
			return osSemaphoreWait( sid_Sem7, ms );
		#if MAX_NUM_SEM == 16
		case 8:
			return osSemaphoreWait( sid_Sem8, ms );
		case 9:
			return osSemaphoreWait( sid_Sem9, ms );
		case 10:
			return osSemaphoreWait( sid_Sem10, ms );
		case 11:
			return osSemaphoreWait( sid_Sem11, ms );
		case 12:
			return osSemaphoreWait( sid_Sem12, ms );
		case 13:
			return osSemaphoreWait( sid_Sem13, ms );
		case 14:
			return osSemaphoreWait( sid_Sem14, ms );
		case 15:
			return osSemaphoreWait( sid_Sem15, ms );
		#endif
		default:
			break;
		
	}
	return ERR_BAD_PARAMETER;
}

int Sem_post(sem_t *sem)
{
	sem_t s = *sem;
	int	ret = 0;
	switch( s)
	{
		case 0:
			ret = osSemaphoreRelease( sid_Sem0);
			break;
		case 1:
			ret = osSemaphoreRelease( sid_Sem1);
			break;
		case 2:
			ret = osSemaphoreRelease( sid_Sem2);
			break;
		case 3:
			ret = osSemaphoreRelease( sid_Sem3);
			break;
		case 4:
			ret = osSemaphoreRelease( sid_Sem4);
			break;
		case 5:
			ret = osSemaphoreRelease( sid_Sem5);
		case 6:
			ret = osSemaphoreRelease( sid_Sem6);
			break;
		case 7:
			ret = osSemaphoreRelease( sid_Sem7);
			break;
		#if MAX_NUM_SEM == 16
		case 8:
			ret = osSemaphoreRelease( sid_Sem8);
			break;
		case 9:
			ret = osSemaphoreRelease( sid_Sem9);
			break;
		case 10:
			ret = osSemaphoreRelease( sid_Sem10);
			break;
		case 11:
			ret = osSemaphoreRelease( sid_Sem11);
			break;
		case 12:
			ret = osSemaphoreRelease( sid_Sem12);
			break;
		case 13:
			ret = osSemaphoreRelease( sid_Sem13);
			break;
		case 14:
			ret = osSemaphoreRelease( sid_Sem14);
			break;
		case 15:
			ret = osSemaphoreRelease( sid_Sem15);
			break;
		#endif
		default:
			return ERR_BAD_PARAMETER;
		
	}
	return ret;
}


int Mutex_init( mutext_t *mutex)
{
	return RET_OK;

}
int Mutex_destory( mutext_t *mutex)
{
	
	
	return RET_OK;
}
int Mutex_lock( mutext_t *mutex)
{
	return RET_OK;
}

int Mutex_trylock( mutext_t *mutex)
{
	return RET_OK;
}

int Mutex_unlock( mutext_t *mutex)
{
	return RET_OK;
}
