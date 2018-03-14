/*
 * os_depend.h
 *
 *  Created on: 2017-1-13
 *      Author: Administrator
 */

#ifndef NOOS_DEPEND_H_
#define NOOS_DEPEND_H_
#include "sdhlibConf.h"
#include <stdint.h>


typedef void (*WasteMs)(int ms);



typedef int os_time_t;
typedef int mutext_t;
typedef uint8_t sem_t;

#endif /* OS_DEPEND_H_ */
