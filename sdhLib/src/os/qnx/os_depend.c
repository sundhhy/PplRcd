/*
 * os_depend.c
 *
 *  Created on: 2017-1-13
 *      Author: Administrator
 */
#include "os/os_depend.h"
#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

void delay_s(int sec)
{
	sleep(sec);
}

void delay_ms(int ms)
{
	delay(ms);
}

void delay_ns( unsigned long nsec )
{
	nanospin_ns(nsec);
}

int get_currenttime(os_time_t *ref_time)
{
	return clock_gettime( CLOCK_REALTIME, ref_time);

}

void PrintTime(os_time_t *time)
{
	printf(" %s \n", ctime(time ));
}

int cal_timediff_ms( os_time_t *ref_time)
{

	struct timespec now_time;
	int diff_ms = 0;
	assert( ref_time != NULL);
	clock_gettime( CLOCK_REALTIME, &now_time);
	diff_ms = ( now_time.tv_sec - ref_time->tv_sec ) * 1000 + \
				( now_time.tv_nsec - ref_time->tv_nsec )/1000000;

	return diff_ms;

}

int Mutex_init( mutext_t *mutex)
{
	if( mutex == NULL)
		return -1;
	return pthread_mutex_init( mutex, NULL);

}
int Mutex_destory( mutext_t *mutex)
{
	return pthread_mutex_destroy( mutex);
}
int Mutex_lock( mutext_t *mutex)
{
	return pthread_mutex_lock( mutex);
}

int Mutex_trylock( mutext_t *mutex)
{
	return pthread_mutex_trylock( mutex);
}

int Mutex_unlock( mutext_t *mutex)
{
	return pthread_mutex_unlock( mutex);
}
