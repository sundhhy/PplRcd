

#ifndef __Ch376_H__
#define __Ch376_H__


#include "config.h"
//#include "target.h"
#include "Ch376INC.h"




#define	ERR_USB_UNKNOWN		(0xFA)	/* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */



#define CH376_CMD_PORT	(*((volatile unsigned char *) 0x82000002))
#define CH376_DATA_PORT	(*((volatile unsigned char *) 0x82000000))

#define SET_CH376RST_LOW	(IO1CLR |= 1<<21)
#define SET_CH376RST_HIGH	(IO1SET |= 1<<21)


#define SET_CH376ENA_HIGH	(IO0SET |= 1<<30)
#define SET_CH376ENA_LOW	(IO0CLR |= 1<<30)


extern	void	DelayMs(unsigned long time);
extern	void	HRst_Ch376(void);
extern	void	Test_USBOCA(void);
extern	uint8	mInitCH376Host(void);

extern	uint8	CH376DiskMount( void );
extern	uint8	CH376ReadBlock( uint8 *buf );
extern	uint8	IsDiskWriteProtect( void );
extern	uint8	CH376DiskQuery( uint32 *DiskFre );
extern	uint8	CH376FileCreate( uint8 *name );
extern	uint8	CH376SecWrite( uint8 *buf, uint8 ReqCount, uint8 *RealCount );
extern	uint8	CH376FileClose( uint8 UpdateSz );


extern	uint8	Ch376DiskConnect(void);
extern	uint8	Ch376GetDiskStatus(void);


extern	uint8	CH376ByteWrite( uint8 *buf, uint16 ReqCount, uint16 *RealCount );


#endif