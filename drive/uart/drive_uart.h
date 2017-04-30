#ifndef __SERAIL_485_UART_H__
#define __SERAIL_485_UART_H__
#include "stdint.h"
#include "stm32f10x_usart.h"
#include "lw_oopc.h"
#include "mem/Ping_PongBuf.h"
#include "dev_cmd.h"


#define SENDMODE_CPU			0
#define SENDMODE_INTR			1
#define SENDMODE_DMA			2
#define SER485_SENDMODE 		SENDMODE_DMA


#define UART_RXCACHE_SIZE		512
#define UART_TXCACHE_SIZE		256

//#define S485_UART_CMD_SET_TXBLOCK	1
//#define S485_UART_CMD_CLR_TXBLOCK	2
//#define S485_UART_CMD_SET_RXBLOCK	3
//#define S485_UART_CMD_CLR_RXBLOCK	4
//#define S485UART_SET_TXWAITTIME_MS	5
//#define S485UART_SET_RXWAITTIME_MS	6

#define DRCT_RX				0
#define DRCT_TX				0

typedef USART_InitTypeDef	ser_485Cfg;
//在串口中断中处理数据，用于需要快速处理的操作
typedef void (*uartIdp)(void *buf, void *arg);
typedef void (*ledHdl)(void );
typedef int (*waitSem)( int ms);
typedef void (*postSem)( void);

typedef  struct usart_control_t {
	short	tx_block;		//阻塞标志
	short	rx_block;
	
	uint16_t	recv_size;
	short	tx_waittime_ms;
	short	rx_waittime_ms;
	
	PPBuf_t	 pingpong;
#if SER485_SENDMODE == SENDMODE_INTR
	//在中断发送的时候，要把正在发送的内存记录下来
	uint8_t		*intrSendingBuf;
	short 		sendingCount;
	short 		sendingLen;
#endif
	
} uartCtl_t;

CLASS( driveUart)
{
	void *rxCache;
	void *txCache;
	void 	*devUart;
	void	*cfg;
	
	ledHdl rxLedHdl, txLedHdl;
	uartIdp	rxIdp, txIdp;
	waitSem rxWait, txWait;
	postSem	rxPost, txPost;
	
	uartCtl_t	ctl;
	
	int ( *init)( driveUart *self, void *dev, void *cfg);
	int ( *deInit)( driveUart *self);
	int ( *read)( driveUart *self, void *buf, int rdLen);
	int	( *takeUpPlayloadBuf)( driveUart *self, void **buf);
	int	( *giveBackPlayloadBuf)( driveUart *self, void *buf);
	int ( *write)( driveUart *self, void *buf, int wrLen);
	int ( *ioctol)( driveUart *self, int cmd, ...);
	
	void (*setLedHdl)( driveUart *self, int rxOrTx, ledHdl hdl);
	void (*setIdp)( driveUart *self, int rxOrTx, uartIdp idp);
	void (*setWaitSem)( driveUart *self, int rxOrTx, waitSem wait);
	void (*setPostSem)( driveUart *self, int rxOrTx, postSem post);
	
	
	int ( *test)( driveUart *self, void *testBuf, int bufSize);
	
	
};


#endif
