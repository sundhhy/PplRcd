

#ifndef __Ch376_H__
#define __Ch376_H__
//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
//#include "config.h"
//#include "target.h"
#include "Ch376INC.h"
#include "hardwareConfig.h"
//------------------------------------------------------------------------------
// check for correct compilation options
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define DTM_CREATE_DATE		0
#define DTM_CHANGE_DATE		1
#define DTM_CREATE_TIME		2
#define DTM_CHANGE_TIME		3
#define DTM_LASTACC_DATE	4

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------
typedef void (*uplevel_intr)(int	arg);
//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

extern int	Init_Ch376(void *op,  uplevel_intr up_irq);






#define	ERR_USB_UNKNOWN		(0xFA)	/* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */







//extern	void	DelayMs(unsigned long time);
//extern	void	HRst_Ch376(void);
//extern void Power_Ch376(int on);
//extern void Ch376_enbale_Irq(int ed);
//extern	void	Test_USBOCA(void);
extern	uint8_t	mInitCH376Host(void);
extern 	void	Search_u_disk(void);
extern	uint8_t	CH376DiskMount( void );
extern  uint8_t	CH376ByteRead(uint8_t* buf, uint16_t ReqCount, uint16_t* RealCount );
extern	uint8_t	CH376ReadBlock( uint8_t *buf, int buf_size );
extern	uint8_t	IsDiskWriteProtect( void );
extern	uint8_t	CH376DiskQuery(uint32_t *DiskAll, uint32_t *DiskFre, uint8_t *diskFat );

extern	uint8_t	CH376FileCreate(char *name );
extern uint8_t	CH376FileOpen(char* name );
extern uint8_t	CH376_Set_Data_Time(int dtm_type, uint16_t	dtm);
extern	uint8_t	CH376SecWrite( uint8_t *buf, uint8_t ReqCount, uint8_t *RealCount );
extern	uint8_t	CH376FileClose( uint8_t is_update_size );


extern	uint8_t	Ch376DiskConnect(void);
//extern	uint8_t	Ch376GetDiskStatus(void);


extern	uint8_t	CH376ByteWrite( uint8_t *buf, uint16_t ReqCount, uint16_t *RealCount );


#endif

