/*
 * os_depend.h
 *
 *  Created on: 2017-1-13
 *      Author: Administrator
 */

#ifndef OS_DEPEND_H_
#define OS_DEPEND_H_
#include "sdhlibConf.h"

#ifdef QNX_OS
#include "os/qnx/qnx_depend.h"
#elif defined NO_OS
#include "os/NoOS_depend.h"
#elif defined RTX_OS
#include "os/rtx/rtx_depend.h"
#endif


void RgtWasteMs( void *wms);

void delay_s(int sec);
void delay_ms(int ms);
void delay_ns( unsigned long nsec );

int get_currenttime(os_time_t *nowtime);
void PrintTime(os_time_t *time);
int cal_timediff_ms( os_time_t *ref_time);
int DiffTimes_ms( os_time_t *p_start, os_time_t *P_end);

int Mutex_init( mutext_t *mutex);
int Mutex_destory( mutext_t *mutex);
int Mutex_lock( mutext_t *mutex);
int Mutex_trylock( mutext_t *mutex);
int Mutex_unlock( mutext_t *mutex);

int Alloc_sem(void);
int Sem_init(sem_t *sem);
int Sem_wait(sem_t *sem, int ms);
int Sem_post(sem_t *sem);
#endif /* OS_DEPEND_H_ */
