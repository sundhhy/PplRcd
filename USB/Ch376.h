

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

//------------------------------------------------------------------------------
// typedef
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global variable declarations
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

extern int	Init_Ch386(int dev_id);






#define	ERR_USB_UNKNOWN		(0xFA)	/* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */





#define SET_CH376RST_LOW	GPIO_SetBits(GPIO_PORT_USBRESET, GPIO_PIN_USBRESET)
#define SET_CH376RST_HIGH	GPIO_ResetBits(GPIO_PORT_USBRESET, GPIO_PIN_USBRESET)


#define SET_CH376ENA_HIGH	GPIO_SetBits(GPIO_PORT_SPI1, GPIO_PIN_SPI1_NSS)
#define SET_CH376ENA_LOW	GPIO_ResetBits(GPIO_PORT_SPI1, GPIO_PIN_SPI1_NSS)


//extern	void	DelayMs(unsigned long time);
extern	void	HRst_Ch376(void);
//extern	void	Test_USBOCA(void);
extern	uint8_t	mInitCH376Host(void);

//extern	uint8_t	CH376DiskMount( void );
//extern	uint8_t	CH376ReadBlock( uint8_t *buf );
//extern	uint8_t	IsDiskWriteProtect( void );
//extern	uint8_t	CH376DiskQuery( uint32 *DiskFre );
//extern	uint8_t	CH376FileCreate( uint8_t *name );
//extern	uint8_t	CH376SecWrite( uint8_t *buf, uint8_t ReqCount, uint8_t *RealCount );
//extern	uint8_t	CH376FileClose( uint8_t UpdateSz );


//extern	uint8_t	Ch376DiskConnect(void);
//extern	uint8_t	Ch376GetDiskStatus(void);


//extern	uint8_t	CH376ByteWrite( uint8_t *buf, uint16 ReqCount, uint16 *RealCount );


#endif

