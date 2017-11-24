#ifndef __DEV_CHAR_H_
#define __DEV_CHAR_H_
#include "stdint.h"
#include "lw_oopc.h"
//字符型设备的接口
//包括uart设备


INTERFACE(I_dev_Char)
{
	int ( *open)( I_dev_Char *self, void *conf);
	int ( *close)( I_dev_Char *self);
	int ( *read)( I_dev_Char *self, void *buf, int rdLen);
	int ( *write)( I_dev_Char *self, void *buf, int wrLen);
	int ( *ioctol)( I_dev_Char *self, int cmd, ...);
	int ( *test)( I_dev_Char *self, void *testBuf, int len);
	
};

int DevChar_open( int major, int minor, void **dev);

#endif
