#ifndef __DEV_CHAR_H_
#define __DEV_CHAR_H_
#include "stdint.h"
#include "lw_oopc.h"
//字符型设备的接口
//包括uart设备

INTERFACE( I_dev_Char)
{
	int ( *open)( void);
	int ( *close)( void);
	int ( *read)( void *buf, int rdLen);
	int ( *write)( void *buf, int wrLen);
	int ( *ioctol)( int cmd, ...);
	int ( *test)( void *testBuf, int len);
	
};

int DevChar_open( int major, int minor, void **dev);

#endif
