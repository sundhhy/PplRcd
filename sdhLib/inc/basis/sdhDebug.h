#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <stdio.h>



//#define DEBUG_SWITCH_ON
#ifdef DEBUG_SWITCH_ON
	#define DPRINTF(format, arg...) printf( format,##arg)
#else 
	#define DPRINTF(format, arg...) 
#endif
#define DEBUG_SWITCH
#ifdef    DEBUG_SWITCH
#define TRACE_INFO(fmt,args...) printf(fmt, ##args)
#else
#define TRACE_INFO(fmt,args...) /*do nothing */
#endif

#define DEBUG_LEVEL_ON		1


/**
 * @brief 打印等级定义
 */

#define 	DDG_EMERG 	0
#define		DDG_ALERT 	1
#define 	DDG_ERR		2
#define		DDG_WARNING	3
#define 	DDG_NOTICE	5
#define		DDG_INFO	6
#define		DDG_DEBUG	7

#define DBG_LEVEL_ALL	0
#define DBG_LEVEL_LL	1
#define DBG_LEVEL_L		2
#define DBG_LEVEL_H		3
#define DBG_LEVEL_HH	4

#define DBG_LEVEL_NONE	5


#define DBG_MIN_LEVEL DBG_LEVEL_L

#if DEBUG_LEVEL_ON
#define LEVEL_DEBUGF( lev, format, arg...) do { \
	if( ( lev > DBG_MIN_LEVEL)) { \
		printf( format,##arg); \
	} \
}while(0)

#else
	#define LEVEL_DEBUGF( lev, format, arg...)

#endif




							 

// 具体项目的测试选项
//PWL组件调试
#define LOOPBACKTEST		1
#define DISABLE_ALLINTR		0
#define USE_TXTHREAD 		0



#endif
