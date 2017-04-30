#include "dev_uart2.h"
#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include <stdarg.h>
#include <stdint.h>

#include "drive_uart.h"

#include "hardwareConfig.h"

#include "cmsis_os.h"                                           // CMSIS RTOS header file

static int Uart2WaitTxSem( int ms);
static int Uart2WaitRxSem( int ms);
static void Uart2PostTxSem( void);
static void Uart2PostRxSem(void);
static void Uart2LedRxHdl(void );
static void Uart2LedTxHdl(void );
static driveUart	*driUart2;

osSemaphoreId sid_Uart2RxSem;                             // semaphore id
osSemaphoreDef(driUart2RxSemaphore);                               // semaphore object
osSemaphoreId sid_Uart2TxSem;                             // semaphore id
osSemaphoreDef(driUart2TxSemaphore);                               // semaphore object




int Dev_Uart2_open( void)
{

	sid_Uart2RxSem = osSemaphoreCreate (osSemaphore(driUart2RxSemaphore), 1);
	if (!sid_Uart2RxSem) {
		LEVEL_DEBUGF( DDG_ERR, "create uart2 txSem failed");
		goto errExit0;
	
	}
	osSemaphoreWait( sid_Uart2RxSem, 0 );
	
	sid_Uart2TxSem = osSemaphoreCreate (osSemaphore(driUart2TxSemaphore), 1);
	if (!sid_Uart2TxSem) {
		LEVEL_DEBUGF( DDG_ERR, "create uart2 txSem failed");
		goto errExit1;
	
	}
	
	
	
	driUart2 = driveUart_new();
	if( driUart2 == NULL)
		goto errExit2;
	
	driUart2->setWaitSem( driUart2, DRCT_RX, Uart2WaitRxSem);
	driUart2->setWaitSem( driUart2, DRCT_TX, Uart2WaitTxSem);
	driUart2->setPostSem( driUart2, DRCT_RX, Uart2PostRxSem);
	driUart2->setPostSem( driUart2, DRCT_TX, Uart2PostTxSem);
	driUart2->setLedHdl( driUart2, DRCT_RX, Uart2LedRxHdl);
	driUart2->setLedHdl( driUart2, DRCT_TX, Uart2LedTxHdl);
	if( driUart2->init( driUart2, USART2, &g_confUart2) != RET_OK)
		goto errExit3;
	
	return RET_OK;
	
	errExit3:
		lw_oopc_delete( driUart2);
	errExit2:
		osSemaphoreDelete( sid_Uart2TxSem);
	
	errExit1:
		osSemaphoreDelete( sid_Uart2RxSem);
	errExit0:
		return ERR_OSRSU_UNAVAILABLE; 
}

int Dev_Uart2_close( void)
{

	
	driUart2->deInit( driUart2);	
	osSemaphoreDelete( sid_Uart2TxSem);
	osSemaphoreDelete( sid_Uart2RxSem);
	return RET_OK; 
}

int Dev_Uart2_read( void *buf, int rdLen)
{
	
	return driUart2->read( driUart2, buf, rdLen);
}

int Dev_Uart2_write( void *buf, int wrLen)
{
	
	return driUart2->write( driUart2, buf, wrLen);
}

int Dev_Uart2_ioctol( int cmd, ...)
{
	
	int int_data;
	va_list arg_ptr; 
	va_start(arg_ptr, cmd); 
	
	switch(cmd)
	{
		case S485_UART_CMD_SET_TXBLOCK:
			driUart2->ctl.tx_block = 1;
			break;
		case S485_UART_CMD_CLR_TXBLOCK:
			driUart2->ctl.tx_block = 0;
			break;
		case S485_UART_CMD_SET_RXBLOCK:
			driUart2->ctl.rx_block = 1;
			break;
		case S485_UART_CMD_CLR_RXBLOCK:
			driUart2->ctl.rx_block = 0;
			driUart2->ctl.rx_waittime_ms = 0;
			break;
		case S485UART_SET_TXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			driUart2->ctl.tx_waittime_ms = int_data;
			break;
		case S485UART_SET_RXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			driUart2->ctl.rx_waittime_ms = int_data;
			break;
		
		
		
		default: break;
		
	}
	
	return RET_OK;
	
//	return driUart2->ioctol( driUart2, cmd,...);
}

int Dev_Uart2_test( void *testBuf, int len)
{
	
	return driUart2->test( driUart2, testBuf, len);
}




static int Uart2WaitTxSem( int ms)
{
	
	return osSemaphoreWait( sid_Uart2TxSem, ms );
}
static int Uart2WaitRxSem( int ms)
{
	
	return osSemaphoreWait( sid_Uart2RxSem, ms );
}

static void Uart2PostTxSem()
{
	
	osSemaphoreRelease( sid_Uart2TxSem );
}
static void Uart2PostRxSem( )
{
	
	osSemaphoreRelease( sid_Uart2RxSem );
}

static void Uart2LedRxHdl( )
{
	
	
}

static void Uart2LedTxHdl( )
{
	
	
}


CTOR( Dev_Uart2)
FUNCTION_SETTING( I_dev_Char.open, Dev_Uart2_open);
FUNCTION_SETTING( I_dev_Char.close, Dev_Uart2_close);
FUNCTION_SETTING( I_dev_Char.read, Dev_Uart2_read);
FUNCTION_SETTING( I_dev_Char.write, Dev_Uart2_write);
FUNCTION_SETTING( I_dev_Char.ioctol, Dev_Uart2_ioctol);
FUNCTION_SETTING( I_dev_Char.test, Dev_Uart2_test);
END_CTOR

