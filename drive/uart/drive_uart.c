/**
* @file 		gprs_uart.c
* @brief		提供为gprs模块服务的串口功能.
* @details	
* @author		sundh
* @date		16-09-20
* @version	A001
* @par Copyright (c): 
* 		XXX??
* @par History:         
*	version: author, date, desc\n
* 	A001:sundh,16-09-20,初始化功能
*/
#include "stdint.h"
#include "uart/drive_uart.h"

#include "basis/sdhError.h"
#include <stdarg.h>
#include <string.h>
#include "sdhDef.h"
#include "mem/CiiMem.h"



#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
		
short		g_sendCount = 0;
short		g_sendLen = 0;
#endif
//用在中断程序中，找到对应的设备
static driveUart	*devArry[ 3];


static void UartDma_Init( driveUart *self);

static int UartInit( driveUart *self, void *device, void *cfg)
{
	CfgUart_t *myCfg = ( CfgUart_t *)cfg;
	devArry[ myCfg->uartNum] = self;
	self->cfg = cfg;
	self->devUartBase = device;
	self->rxCache = calloc( 1, UART_RXCACHE_SIZE);
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	self->txCache = calloc( 1, UART_TXCACHE_SIZE);
#endif
	
	init_pingponfbuf( &self->ctl.pingpong, self->rxCache, UART_RXCACHE_SIZE, TURE);
	USART_Cmd( self->devUartBase, DISABLE);
	USART_DeInit( device);
	
	
	USART_Init( device, myCfg->cfguart);
	UartDma_Init( self);

	
	USART_ClearFlag( device,USART_IT_IDLE );
	
	USART_ITConfig( device, USART_IT_RXNE, ENABLE);
	USART_ITConfig( device, USART_IT_IDLE, ENABLE);
	
	
#if SER485_SENDMODE == SENDMODE_DMA	
	USART_DMACmd( device, USART_DMAReq_Tx, ENABLE);  // 开启DMA发送
#endif
	USART_DMACmd( device, USART_DMAReq_Rx, ENABLE); // 开启DMA接收
	
	USART_Cmd( device, ENABLE);
	self->ctl.rx_block = 1;
	self->ctl.tx_block = 1;
	self->ctl.rx_waittime_ms = 100;
	self->ctl.tx_waittime_ms = 1000;
	
	return ERR_OK;
	
}


static int UartDeInit( driveUart *self)
{
	
	free( self->rxCache);
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	free( self->txCache);
#endif
	
	USART_Cmd( self->devUartBase, DISABLE);
	USART_DeInit( self->devUartBase);
	

	
	self->rxLedHdl = NULL;
	self->txLedHdl = NULL;
	self->rxIdp = NULL;
	self->txIdp = NULL;
	self->rxWait = NULL;
	self->txWait = NULL;
	self->rxPost = NULL;
	self->txPost = NULL;
	
	return ERR_OK;
	
}

/*!
**
**
** @param data 
** @param size 
** @return
**/
static int UartWrite( driveUart *self, void *buf, int wrLen)
{
	CfgUart_t *myCfg = ( CfgUart_t *)self->cfg;
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	int ret;
	char *sendbuf ;
#else
	int count = 0;
#endif
	if( buf == NULL || wrLen == 0)
		return ERR_BAD_PARAMETER;
	if( self->status == UARTSTATUS_TXBUSY)
		return ERR_BUSY;
	
	
	self->ioctol( self, DRICMD_SET_DIR_TX);
	
	
#if ( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)
	if( wrLen  < UART_TXCACHE_SIZE)
	{
		memset( self->txCache, 0, UART_TXCACHE_SIZE);
		memcpy( self->txCache, buf, wrLen);
		sendbuf = self->txCache;
	
	}
	else
	{
		sendbuf = buf;
		
	}
	
	
#	if SER485_SENDMODE == SENDMODE_DMA	
	
	myCfg->dma->dma_tx_base->CMAR = (uint32_t)sendbuf;
	myCfg->dma->dma_tx_base->CNDTR = (uint16_t)wrLen; 
	DMA_Cmd( myCfg->dma->dma_tx_base, ENABLE);        //开始DMA发送
//	USART_ITConfig( self->devUartBase, USART_IT_TXE, ENABLE);
#	elif SER485_SENDMODE == SENDMODE_INTR		
	
	self->ctl.intrSendingBuf = sendbuf;
	self->ctl.sendingCount = 1;
	self->ctl.sendingLen = size;
	USART_SendData( self->devUartBase, sendbuf[0]);
	USART_ITConfig( self->devUartBase, USART_IT_TXE, ENABLE);

#	endif
//	osDelay(1);
	if( self->ctl.tx_block)
	{
		if( self->txWait)
		{
			
			ret = self->txWait( self, self->ctl.tx_waittime_ms);
		}
		
		
		if ( ret > 0) 
		{

			return ERR_OK;
		}
		
		return ERR_DEV_TIMEOUT;
		
	}
	
#endif	//( SER485_SENDMODE == SENDMODE_INTR ) || ( SER485_SENDMODE == SENDMODE_DMA)

#if SER485_SENDMODE == SENDMODE_CPU		
	while( count < size)
	{
		USART_SendData( self->devUartBase, data[count]);
		while( USART_GetFlagStatus( self->devUartBase, USART_FLAG_TXE) == RESET){};
		count ++;
	}

#endif	
	return ERR_OK;
}



/*!
**
**
** @param data 
** @param size 
** @return
**/
static int UartRead( driveUart *self, void *buf, int rdLen)
{
	int  ret = 1;
	int len = rdLen;
	char *playloadbuf ;
	
	if( buf == NULL)
		return ERR_BAD_PARAMETER;
	

	
	if( self->rxWait)
	{
		
		ret = self->rxWait( self, self->ctl.rx_waittime_ms);
	}
	
	if( ret > 0)
	{
		if( len > self->ctl.recv_size)
			len = self->ctl.recv_size;
		playloadbuf = get_playloadbuf( &self->ctl.pingpong);
		memset( buf, 0, rdLen);
		memcpy( buf, playloadbuf, len);
		memset( playloadbuf, 0, len);
		free_playloadbuf( &self->ctl.pingpong);
//		LED_com->turnon(LED_com);
//		if( T485Rxirq_cb != NULL && len)
//			T485Rxirq_cb->cb( NULL,  T485Rxirq_cb->arg);
		self->ctl.recv_size = 0;
		return len;
	}
	
	return 0;
}

//将串口的缓存给调用者
//返回当前缓存的数据长度
static int UartTakeUpPlayloadBuf( driveUart *self, void **data)
{
	int  ret = 1;
	int len = 0;
	char *playloadbuf ;

	if( self->rxWait)
	{
		
		ret = self->rxWait( self, self->ctl.rx_waittime_ms);
	}
	
	if( ret > 0)
	{
		
		len = self->ctl.recv_size;
		playloadbuf = get_playloadbuf( &self->ctl.pingpong);
		self->ctl.recv_size = 0;
		*data = playloadbuf;
		return len;
	}
	
	return 0;
}

int UartGiveBackPlayloadBuf( driveUart *self, void *data)
{
	free_playloadbuf( &self->ctl.pingpong);
	return 0;
}


/*!
**
**
** @param size
**
** @return
**/
static void UartIoctol( driveUart *self, int cmd, ...)
{
	int int_data;
	va_list arg_ptr; 
	va_start(arg_ptr, cmd); 
	
	switch(cmd)
	{
		case DRICMD_DISABLE_TXBLOCK:
			self->ctl.tx_block = 0;
			break;
		case DRICMD_ENABLE_TXBLOCK:
			self->ctl.tx_block = 1;
			break;
		case DRICMD_DISABLE_RXBLOCK:
			self->ctl.rx_block = 0;
			self->ctl.rx_waittime_ms = 0;
			break;
		case DRICMD_ENABLE_RXBLOCK:
			self->ctl.rx_block = 1;
			
			break;
		case DRICMD_SET_TXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			self->ctl.tx_waittime_ms = int_data;
			break;
		case DRICMD_SET_RXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			self->ctl.rx_waittime_ms = int_data;
			break;
		case DRICMD_SET_DIR_RX:
			if( self->dirPin->Port)
			{
				GPIO_ResetBits( self->dirPin->Port, self->dirPin->pin);
				
			}
			break;
		case DRICMD_SET_DIR_TX:
			if( self->dirPin->Port)
			{
				GPIO_SetBits( self->dirPin->Port, self->dirPin->pin);
				
			}
			break;
			

		
		
		default: break;
		
	}
}




static int UartTest( driveUart *self, void *buf, int size)
{
	char *pp = NULL;
	char	*pdata = NULL;
  int len;
	
	strcpy( buf, "Serial 485 test\n" );
	self->ioctol( self, DRICMD_SET_TXWAITTIME_MS, 10);
	self->ioctol( self, DRICMD_SET_RXWAITTIME_MS, 10);
	
	
	self->write( self, buf, strlen(buf));
	while(1)
	{
		
	
		len = self->read( self, buf, size);
		pp = strstr((const char*)buf,"OK");
		if(pp)
			return ERR_OK;
		
		if( len > 0)
		{
			self->write( self, buf, len);
		}
		
//		len = self->takeUpPlayloadBuf( self, ( void *)&pdata);
//		pp = strstr((const char*)pdata,"OK");
//		if(pp)
//			return ERR_OK;
//		
//		if( len > 0)
//		{
//			self->write( self, pdata, len);
//		}
//		
//		self->giveBackPlayloadBuf( self, ( void *)pdata);
		
		
		
	}
	
}


static void UartSetLedHdl( driveUart *self, int rxOrTx, ledHdl hdl)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxLedHdl = hdl;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txLedHdl = hdl;
	} 
	
	
}

static void UartSetIdp( driveUart *self, int rxOrTx, uartIdp idp, void *arg)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxIdp = idp;
		self->argRxIdp = arg;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txIdp = idp;
		self->argTxIdp = arg;
	} 
	
	
}
static void UartSetWaitSem( driveUart *self, int rxOrTx, waitSem wait)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxWait = wait;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txWait = wait;
	} 
	
	
}
static void UartSetPostSem( driveUart *self, int rxOrTx, postSem post)
{
	if( rxOrTx == DRCT_RX)
	{
		self->rxPost = post;
	}
	else if( rxOrTx == DRCT_TX)
	{
		self->txPost = post;
	} 
	
}

/*! gprs uart dma Configuration*/
void UartDma_Init( driveUart *self)
{
	CfgUart_t *myCfg = ( CfgUart_t *)self->cfg;
	USART_TypeDef *devUartBase = ( USART_TypeDef *)self->devUartBase;
	
	DMA_InitTypeDef DMA_InitStructure;	
	short rxbuflen;
	char *rxbuf;

    
//=DMA_Configuration==============================================================================//	
/*--- UART_Tx_DMA_Channel DMA Config ---*/
#if SER485_SENDMODE == SENDMODE_DMA	

    DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE);                           // 关闭DMA
    DMA_DeInit( myCfg->dma->dma_tx_base);                                 // 恢复初始值
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&devUartBase->DR);// 外设地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)self->txCache;        
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                      // 从内存到外设
    DMA_InitStructure.DMA_BufferSize = UART_TXCACHE_SIZE;                    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址增加
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度1B
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存地址宽度1B
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // 单次传输模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;                 // 优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            // 关闭内存到内存模式
    DMA_Init( myCfg->dma->dma_tx_base, &DMA_InitStructure);               // 
    
	DMA_ClearFlag( myCfg->dma->dma_tx_flag );                                 // 清除标志
	DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE); 												// 关闭DMA
    DMA_ITConfig( myCfg->dma->dma_tx_base, DMA_IT_TC, ENABLE);            // 允许传输完成中断

 #endif  

/*--- UART_Rx_DMA_Channel DMA Config ---*/

 
	switch_receivebuf( &self->ctl.pingpong, &rxbuf, &rxbuflen);
    DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE);                           
    DMA_DeInit( myCfg->dma->dma_rx_base);                                 
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&devUartBase->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxbuf;         
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                     
    DMA_InitStructure.DMA_BufferSize = rxbuflen;                     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;                 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            
    DMA_Init( myCfg->dma->dma_rx_base, &DMA_InitStructure);               
    DMA_ClearFlag( myCfg->dma->dma_rx_flag);      
//	DMA_ITConfig(DMA_s485_usart.dma_rx_base, DMA_IT_TC, ENABLE); 	 // 允许传输完成中断

    DMA_Cmd( myCfg->dma->dma_rx_base, ENABLE);                            

   

}

void Usart_irq( driveUart *thisDev)
{
	short rxbuflen;
	char *rxbuf;
	uint8_t clear_idle = clear_idle;
	USART_TypeDef *devUartBase = ( USART_TypeDef *)thisDev->devUartBase;
	//这个500是在115200的波特率，CPU时钟是48M的情况下通过逐次逼近测得的
//	int i = 500;
	
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
	if(USART_GetITStatus( thisDev->devUartBase, USART_IT_IDLE) != RESET)  // 空闲中断
	{
		
		DMA_Cmd( myCfg->dma->dma_rx_base, DISABLE);       // 关闭DMA
		DMA_ClearFlag(  myCfg->dma->dma_rx_flag );           // 清除DMA标志
		thisDev->ctl.recv_size = get_loadbuflen( &thisDev->ctl.pingpong)  - \
		DMA_GetCurrDataCounter(  myCfg->dma->dma_rx_base); //获得接收到的字节
		
		switch_receivebuf( &thisDev->ctl.pingpong, &rxbuf, &rxbuflen);
		myCfg->dma->dma_rx_base->CMAR = (uint32_t)rxbuf;
		myCfg->dma->dma_rx_base->CNDTR = rxbuflen;
		DMA_Cmd(  myCfg->dma->dma_rx_base, ENABLE);
		
	
		clear_idle = devUartBase->SR;
		clear_idle = devUartBase->DR;
		USART_ReceiveData( devUartBase); // Clear IDLE interrupt flag bit


		if( thisDev->rxLedHdl)
			thisDev->rxLedHdl(thisDev);
		if( thisDev->rxPost)
			thisDev->rxPost(thisDev);
		if( thisDev->rxIdp)
			thisDev->rxIdp( thisDev->argRxIdp);
		
	}
	if(USART_GetITStatus( thisDev->devUartBase, USART_IT_RXNE) == SET) 
	{
		//清除RXNE标志，防止在接收的数据长度超过dma的接收长度之后，没有任何应用去接收数据而造成串口永远处于中断状态
		USART_ReceiveData( thisDev->devUartBase);
	}
	//这个中断在DMA发送完成中断中开启
	if(USART_GetITStatus( thisDev->devUartBase, USART_IT_TC) == SET) 
	{
		USART_ClearFlag( thisDev->devUartBase,USART_IT_TC );
		USART_ITConfig( thisDev->devUartBase, USART_IT_TC, DISABLE);
		
		if( thisDev->txPost)
			thisDev->txPost(thisDev);
		if( thisDev->txIdp)
			thisDev->txIdp( thisDev->argTxIdp);
		
		thisDev->ioctol( thisDev, DRICMD_SET_DIR_RX);
		thisDev->status = UARTSTATUS_IDLE;
		
	}
	
#if SER485_SENDMODE == SENDMODE_INTR	
	if(USART_GetITStatus( thisDev->devUartBase, USART_IT_TXE) == SET)  // 发送中断
	{
		

		thisDev->ctl.sendingCount ++;
			
	
		//发送完成后关闭发展中断，避免发送寄存器空了就会产生中断
		if( thisDev->ctl.sendingCount >= thisDev->ctl.sendingLen)
		{
			USART_ITConfig( thisDev->devUartBase, USART_IT_TXE, DISABLE);
			if( thisDev->txPost)
				thisDev->txPost(thisDev);
		}			
		else
			USART_SendData( thisDev->devUartBase, thisDev->ctl.intrSendingBuf[ thisDev->ctl.sendingCount] );
		
	
		
	}
#endif	
	

	
}

void USART1_IRQHandler(void)
{
	
	driveUart	*thisDev = devArry[0];
	
	Usart_irq( thisDev);
}


//中断处理程序根据资源与串口的绑定情况来选择设备
void DMA1_Channel4_IRQHandler(void)
{
	driveUart	*thisDev = devArry[0];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
//	int i = 1000;
	if(DMA_GetITStatus(DMA1_FLAG_TC4))		//2是发送中断
	{
		
		DMA_ClearFlag( myCfg->dma->dma_tx_flag);         // 清除标志
		DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE);   // 关闭DMA通道
		
		//等到硬件中所有的寄存器都移入串口
//		while(!USART_GetFlagStatus( thisDev->devUartBase, USART_FLAG_TXE));
		
		//还有数据未完成发送
//		if( USART_GetFlagStatus( thisDev->devUartBase, USART_FLAG_TXE))
//		{
			USART_ClearFlag( thisDev->devUartBase,USART_IT_TC );
			USART_ITConfig( thisDev->devUartBase, USART_IT_TC, ENABLE);
//		}
//		else
//		{
//			
//			thisDev->ioctol( thisDev, DRICMD_SET_DIR_RX);
//		}
		
//		if( thisDev->txPost)
//			thisDev->txPost(thisDev);
//		if( thisDev->txIdp)
//			thisDev->txIdp( thisDev->argTxIdp);
//		thisDev->status = UARTSTATUS_IDLE;
//		while(i)
//			i --;
//		thisDev->ioctol( thisDev, DRICMD_SET_DIR_RX);
		
	
	}
}


void USART2_IRQHandler(void)
{
	
	driveUart	*thisDev = devArry[1];
	Usart_irq( thisDev);


}


//中断处理程序根据资源与串口的绑定情况来选择设备
void DMA1_Channel7_IRQHandler(void)
{
	driveUart	*thisDev = devArry[1];
	CfgUart_t *myCfg = ( CfgUart_t *)thisDev->cfg;
	if(DMA_GetITStatus(DMA1_FLAG_TC7))
	{

		DMA_ClearFlag( myCfg->dma->dma_tx_flag);         // 清除标志
		DMA_Cmd( myCfg->dma->dma_tx_base, DISABLE);   // 关闭DMA通道
		
		USART_ClearFlag( thisDev->devUartBase,USART_IT_TC );
		USART_ITConfig( thisDev->devUartBase, USART_IT_TC, ENABLE);
		
//		while( USART_GetFlagStatus( thisDev->devUartBase, USART_FLAG_TXE) == RESET){};
//		if( thisDev->txPost)
//			thisDev->txPost(thisDev);

//		if( thisDev->txIdp)
//			thisDev->txIdp( thisDev->argTxIdp);
//		thisDev->status = UARTSTATUS_IDLE;
//		thisDev->ioctol( thisDev, DRICMD_SET_DIR_RX);
		
	}
}


CTOR( driveUart)
FUNCTION_SETTING( init, UartInit);
FUNCTION_SETTING( deInit, UartDeInit);
FUNCTION_SETTING( read, UartRead);
FUNCTION_SETTING( takeUpPlayloadBuf, UartTakeUpPlayloadBuf);
FUNCTION_SETTING( giveBackPlayloadBuf, UartGiveBackPlayloadBuf);
FUNCTION_SETTING( write, UartWrite);
FUNCTION_SETTING( ioctol, UartIoctol);
FUNCTION_SETTING( setLedHdl, UartSetLedHdl);
FUNCTION_SETTING( setIdp, UartSetIdp);
FUNCTION_SETTING( setWaitSem, UartSetWaitSem);
FUNCTION_SETTING( setPostSem, UartSetPostSem);
FUNCTION_SETTING( test, UartTest);
END_CTOR

