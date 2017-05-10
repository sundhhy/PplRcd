#include "dev_uart1.h"
#include "basis/sdhDebug.h"
#include "sdhDef.h"
#include <stdarg.h>
#include <stdint.h>

#include "drive_uart.h"

#include "hardwareConfig.h"

#include "cmsis_os.h"                                           // CMSIS RTOS header file

static int Uart1WaitTxSem( int ms);
static int Uart1WaitRxSem( int ms);
static void Uart1PostTxSem( void);
static void Uart1PostRxSem(void);
static void Uart1LedRxHdl(void );
static void Uart1LedTxHdl(void );
static driveUart	*driUart1;
static Dev_Uart1 *devUart1;
osSemaphoreId sid_Uart1RxSem;                             // semaphore id
osSemaphoreDef(driUart1RxSemaphore);                               // semaphore object
osSemaphoreId sid_Uart1TxSem;                             // semaphore id
osSemaphoreDef(driUart1TxSemaphore);                               // semaphore object


Dev_Uart1 *Get_DevUart1(void)
{
	I_dev_Char *devChar;
	if( driUart1 == NULL)
	{
		devUart1 = Dev_Uart1_new();
		devChar = ( I_dev_Char *)devUart1;
		devChar->open();
		
	}
	return devUart1;
}

int Dev_Uart1_open( void)
{

	sid_Uart1RxSem = osSemaphoreCreate (osSemaphore(driUart1RxSemaphore), 1);
	if (!sid_Uart1RxSem) {
		LEVEL_DEBUGF( DDG_ERR, "create uart1 txSem failed");
		goto errExit0;
	
	}
	osSemaphoreWait( sid_Uart1RxSem, 0 );
	
	sid_Uart1TxSem = osSemaphoreCreate (osSemaphore(driUart1TxSemaphore), 1);
	if (!sid_Uart1TxSem) {
		LEVEL_DEBUGF( DDG_ERR, "create uart1 txSem failed");
		goto errExit1;
	
	}
	
	
	
	driUart1 = driveUart_new();
	if( driUart1 == NULL)
		goto errExit2;
	
	driUart1->setWaitSem( driUart1, DRCT_RX, Uart1WaitRxSem);
	driUart1->setWaitSem( driUart1, DRCT_TX, Uart1WaitTxSem);
	driUart1->setPostSem( driUart1, DRCT_RX, Uart1PostRxSem);
	driUart1->setPostSem( driUart1, DRCT_TX, Uart1PostTxSem);
	driUart1->setLedHdl( driUart1, DRCT_RX, Uart1LedRxHdl);
	driUart1->setLedHdl( driUart1, DRCT_TX, Uart1LedTxHdl);
	if( driUart1->init( driUart1, USART1, &g_confUart1) != RET_OK)
		goto errExit3;
	
	return RET_OK;
	
	errExit3:
		lw_oopc_delete( driUart1);
	errExit2:
		osSemaphoreDelete( sid_Uart1TxSem);
	
	errExit1:
		osSemaphoreDelete( sid_Uart1RxSem);
	errExit0:
		return ERR_OSRSU_UNAVAILABLE; 
}

int Dev_Uart1_close( void)
{

	
	driUart1->deInit( driUart1);	
	osSemaphoreDelete( sid_Uart1TxSem);
	osSemaphoreDelete( sid_Uart1RxSem);
	lw_oopc_delete( driUart1);
	driUart1 = NULL;
	return RET_OK; 
}

int Dev_Uart1_read( void *buf, int rdLen)
{
	
	return driUart1->read( driUart1, buf, rdLen);
}

int Dev_Uart1_write( void *buf, int wrLen)
{
	
	return driUart1->write( driUart1, buf, wrLen);
}

int Dev_Uart1_ioctol( int cmd, ...)
{
	
	int int_data;
	va_list arg_ptr; 
	va_start(arg_ptr, cmd); 
	
	switch(cmd)
	{
		case S485_UART_CMD_SET_TXBLOCK:
			driUart1->ctl.tx_block = 1;
			break;
		case S485_UART_CMD_CLR_TXBLOCK:
			driUart1->ctl.tx_block = 0;
			break;
		case S485_UART_CMD_SET_RXBLOCK:
			driUart1->ctl.rx_block = 1;
			break;
		case S485_UART_CMD_CLR_RXBLOCK:
			driUart1->ctl.rx_block = 0;
			driUart1->ctl.rx_waittime_ms = 0;
			break;
		case S485UART_SET_TXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			driUart1->ctl.tx_waittime_ms = int_data;
			break;
		case S485UART_SET_RXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			driUart1->ctl.rx_waittime_ms = int_data;
			break;
		
		
		
		default: break;
		
	}
	
	return RET_OK;
	
//	return driUart1->ioctol( driUart1, cmd,...);
}

int Dev_Uart1_test( void *testBuf, int len)
{
	
	return driUart1->test( driUart1, testBuf, len);
}




static int Uart1WaitTxSem( int ms)
{
	
	return osSemaphoreWait( sid_Uart1TxSem, ms );
}
static int Uart1WaitRxSem( int ms)
{
	
	return osSemaphoreWait( sid_Uart1RxSem, ms );
}

static void Uart1PostTxSem()
{
	
	osSemaphoreRelease( sid_Uart1TxSem );
}
static void Uart1PostRxSem( )
{
	
	osSemaphoreRelease( sid_Uart1RxSem );
}

static void Uart1LedRxHdl( )
{
	
	
}

static void Uart1LedTxHdl( )
{
	
	
}


CTOR( Dev_Uart1)
FUNCTION_SETTING( I_dev_Char.open, Dev_Uart1_open);
FUNCTION_SETTING( I_dev_Char.close, Dev_Uart1_close);
FUNCTION_SETTING( I_dev_Char.read, Dev_Uart1_read);
FUNCTION_SETTING( I_dev_Char.write, Dev_Uart1_write);
FUNCTION_SETTING( I_dev_Char.ioctol, Dev_Uart1_ioctol);
FUNCTION_SETTING( I_dev_Char.test, Dev_Uart1_test);
END_CTOR

