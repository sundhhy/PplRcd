/*
 * os_depend.c
 *
 *  Created on: 2017-1-13
 *      Author: Administrator
 */
#include "os/os_depend.h"
#include <assert.h>
#include <stdio.h>

WasteMs Runms = NULL;

static void Run_ms(int ms)
{
	int i = 0;
	i = CPU_CLOCK / 1000;
	i = i * ms;
	for( ; i > 0; i--)
	{
		;
	}
	
}

void RgtWasteMs( void * wms)
{
	
	Runms = ( WasteMs)wms;
}


void delay_s(int sec)
{
}

void delay_ms(int ms)
{
//	int i = 0;
	
	
		if( Runms)
			Runms(ms);
		else
			Run_ms(ms);
	
	
}

void delay_ns( unsigned long nsec )
{
}

int get_currenttime(os_time_t *ref_time)
{
	return 0;
}

void PrintTime(os_time_t *time)
{
	
}

int cal_timediff_ms( os_time_t *ref_time)
{

	int diff_ms = 0;
	

	return diff_ms;

}

int Mutex_init( mutext_t *mutex)
{
	
	return 0;
}
int Mutex_destory( mutext_t *mutex)
{
	return 0;
}
int Mutex_lock( mutext_t *mutex)
{
	return 0;
}

int Mutex_trylock( mutext_t *mutex)
{
	return 0;
}

int Mutex_unlock( mutext_t *mutex)
{
	return 0;
}
