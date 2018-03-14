/*
 * os_depend.h
 *
 *  Created on: 2017-1-13
 *      Author: Administrator
 */

#ifndef RTX_DEPEND_H_
#define RTX_DEPEND_H_
#include <stdint.h>
#include "cmsis_os.h" 
#define FOREVER		osWaitForever
//#include "osObjects.h"                      // RTOS object definitions
typedef unsigned int os_time_t;
typedef int mutext_t;
typedef int sem_t;


#endif /* OS_DEPEND_H_ */
