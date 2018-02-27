#ifndef __SERAIL_485_UART_H__
#define __SERAIL_485_UART_H__
#include "stdint.h"
#include "stm32f10x_usart.h"
#include "lw_oopc.h"
#include "mem/Ping_PongBuf.h"
#include "dri_cmd.h"
#include "hardwareConfig.h"





#define NUM_UARTS		4
#define UART_RXCACHE_SIZE		512
#define UART_TXCACHE_SIZE		64


#define DRCT_RX				0
#define DRCT_TX				1


#define UARTSTATUS_IDLE				0
#define UARTSTATUS_TXBUSY			1

typedef USART_InitTypeDef	ser_485Cfg;

//在串口中断中处理数据，用于需要快速处理的操作
typedef void (*uartIdp)( void *buf);
typedef void (*ledHdl)(void *self);
typedef int (*waitSem)( void *self, int ms);
typedef void (*postSem)( void* self);


typedef  struct usart_control_t {
	short	tx_block;		//阻塞标志
	short	rx_block;
	
	uint16_t	recv_size;
	short	tx_waittime_ms;
	short	rx_waittime_ms;
	
	PPBuf_t	 pingpong;

	uint8_t		*intrSendingBuf;
	short 		sendingCount;
	short 		sendingLen;

	
} uartCtl_t;

CLASS( driveUart)
{
	uint8_t *rxCache;
	uint8_t *txCache;
	void 	*devUartBase;
	void	*cfg;
	void	*device;
	gpio_pins			*dirPin;
	
	ledHdl rxLedHdl, txLedHdl;
	uartIdp	rxIdp, txIdp;
	void	*argRxIdp, *argTxIdp;
	waitSem rxWait, txWait;
	postSem	rxPost, txPost;
	
	uartCtl_t	ctl;
	
	uint8_t			status;
	uint8_t			none[3];
	
	int ( *init)( driveUart *self, void *cfg);
	int ( *deInit)( driveUart *self);
	int ( *read)( driveUart *self, void *buf, int rdLen);
	int	( *takeUpPlayloadBuf)( driveUart *self, void **buf);
	int	( *giveBackPlayloadBuf)( driveUart *self, void *buf);
	int ( *write)( driveUart *self, void *buf, int wrLen);
	int ( *ioctol)( driveUart *self, int cmd, ...);
	
	void (*setLedHdl)( driveUart *self, int rxOrTx, ledHdl hdl);
	void (*setIdp)( driveUart *self, int rxOrTx, uartIdp idp, void *arg);
	void (*setWaitSem)( driveUart *self, int rxOrTx, waitSem wait);
	void (*setPostSem)( driveUart *self, int rxOrTx, postSem post);
	
	
	int ( *test)( driveUart *self, void *testBuf, int bufSize);
	
	
};

void DRI_Uart_Change_baud(driveUart *p_dri, int new_baud);


#endif
