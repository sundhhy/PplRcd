#include "dev_uart.h"
#include "basis/sdhDebug.h"
#include "basis/macros.h"
#include "sdhDef.h"
#include <stdarg.h>
#include <stdint.h>
#include "dev_cmd.h"
#include "os/os_depend.h"
#include "hardwareConfig.h"


static int UartWaitTxSem( void *self, int ms);
static int UartWaitRxSem( void *self, int ms);
static void UartPostTxSem( void *self);
static void UartPostRxSem(void *self);
static void UartLedRxHdl(void *self);
static void UartLedTxHdl(void *self);
static Dev_Uart *devUart[NUM_UARTS];



Dev_Uart *Get_DevUart( int minor)
{
//	uint8_t	*prxSem, *ptxSem;
//	uint8_t	*p_lock;
	I_dev_Char *devChar;
	if( ( minor +1) > NUM_UARTS)
		return NULL;
	
	if( devUart[ minor] == NULL)
	{
		devUart[ minor] = Dev_Uart_new();
		if(devUart[ minor]  == NULL) while(1);
		devChar = ( I_dev_Char *)devUart[ minor];
		
		devUart[ minor]->dri = driveUart_new();
		if(devUart[ minor]  == NULL) while(1);
		devUart[ minor]->minor = minor;
		
		
		
//		prxSem =  malloc(1);
//		ptxSem = malloc(1);
//		*prxSem = minor * 2;
//		*ptxSem = minor * 2 + 1;
//		if( Sem_init(prxSem))
//			goto errExit0;
//		if( Sem_init(ptxSem))
//			goto errExit1;
//		devUart[ minor]->rxsem = prxSem;
//		devUart[ minor]->txsem = ptxSem;
		
		devUart[ minor]->txsem = Alloc_sem();
		devUart[ minor]->rxsem = Alloc_sem();
		
		Sem_init(&devUart[ minor]->txsem);
		
		Sem_init(&devUart[ minor]->rxsem);
			
		
		devUart[ minor]->dri->device = devUart[ minor];
		

		//在这里进行实际的设备初始化，是为了避免每次打开设备的时候都进行设备初始化
		//todo: 可以想个更好的方式，暂时就这么处理
		if( minor == 0)
		{
			devUart[ minor]->dri->devUartBase = USART1;
			devUart[ minor]->dri->dirPin = NULL;
			devChar->open(devChar, &g_confUart1);	
		}
		else if( minor == 1) 
		{
			devUart[ minor]->dri->devUartBase = USART2;
			devUart[ minor]->dri->dirPin = NULL;
			devChar->open(devChar, &g_confUart2);	
		}
		else if( minor == 2) 
		{
			devUart[ minor]->dri->devUartBase = USART3;
			devUart[ minor]->dri->dirPin = NULL;
			devChar->open(devChar, &g_confUart3);	
		}
		else if( minor == 3) 
		{
			devUart[ minor]->dri->devUartBase = UART4;
			devUart[ minor]->dri->dirPin = NULL;
			devChar->open(devChar, &g_confUart4);	
		}
		
	}
	return devUart[minor];
	
//	errExit0:
//	
//	errExit1:
	
//	return NULL;
}

int Dev_Uart_open( I_dev_Char *self, void *conf)
{
	Dev_Uart *cthis = ( Dev_Uart *)self;
	driveUart *driUart = cthis->dri;

	if( driUart == NULL)
		goto errExit1;
	
	driUart->setWaitSem( driUart, DRCT_RX, UartWaitRxSem);
	driUart->setWaitSem( driUart, DRCT_TX, UartWaitTxSem);
	driUart->setPostSem( driUart, DRCT_RX, UartPostRxSem);
	driUart->setPostSem( driUart, DRCT_TX, UartPostTxSem);
	driUart->setLedHdl( driUart, DRCT_RX, UartLedRxHdl);
	driUart->setLedHdl( driUart, DRCT_TX, UartLedTxHdl);
	if( driUart->init( driUart, conf) != RET_OK)
		goto errExit2;
	
	driUart->ioctol( driUart, DRICMD_SET_DIR_RX);
	return RET_OK;

	errExit2:
		lw_oopc_delete( driUart);
	errExit1:
		return ERR_OSRSU_UNAVAILABLE; 
}

int Dev_Uart_close( I_dev_Char *self)
{
	Dev_Uart *cthis = ( Dev_Uart *)self;
	driveUart *driUart = cthis->dri;
	int minor = cthis->minor;
	driUart->deInit( driUart);	
	
	lw_oopc_delete( driUart);
	driUart = NULL;
	lw_oopc_delete ( cthis);
	devUart[ minor] = NULL;
	return RET_OK; 
}

int Dev_Uart_read( I_dev_Char *self, void *buf, int rdLen)
{
	Dev_Uart *cthis = ( Dev_Uart *)self;
	driveUart *driUart = cthis->dri;
	return driUart->read( driUart, buf, rdLen);
}

int Dev_Uart_write( I_dev_Char *self, void *buf, int wrLen)
{
	Dev_Uart *cthis = ( Dev_Uart *)self;
	driveUart *driUart = cthis->dri;
	int			ret = 0;
//	Sem_wait(cthis->p_lock, FOREVER);
	ret = driUart->write( driUart, buf, wrLen);
//	Sem_post(cthis->p_lock);
	
	return ret;
}

int Dev_Uart_ioctol( I_dev_Char *self ,int cmd, ...)
{
	
	Dev_Uart *cthis = ( Dev_Uart *)self;
	driveUart *driUart = cthis->dri;
	uartIdp	idp;
	void	*arg;
	void	**ppdata;
	int int_data;
	va_list arg_ptr; 
	va_start(arg_ptr, cmd); 
	
	switch(cmd)
	{
		case DEVCMD_DISABLE_TXBLOCK:
			driUart->ioctol( driUart, DRICMD_DISABLE_TXBLOCK);
			
			break;
		case DEVCMD_ENABLE_TXBLOCK:
			driUart->ioctol( driUart, DRICMD_ENABLE_TXBLOCK);
			break;
		case DEVCMD_DISABLE_RXBLOCK:
			driUart->ioctol( driUart, DRICMD_DISABLE_RXBLOCK);
			break;
		case DEVCMD_ENABLE_RXBLOCK:
			driUart->ioctol( driUart, DRICMD_ENABLE_RXBLOCK);
			break;
		case DEVCMD_SET_TXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			driUart->ioctol( driUart, DRICMD_SET_TXWAITTIME_MS, int_data);
			break;
		case DEVCMD_SET_RXWAITTIME_MS:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			driUart->ioctol( driUart, DRICMD_SET_RXWAITTIME_MS, int_data);
			break;
		case DEVCMD_SET_RXIRQHDL:
			idp =  va_arg(arg_ptr, uartIdp);
			arg = va_arg(arg_ptr, void*);
			va_end(arg_ptr); 
			driUart->setIdp( driUart, DRCT_RX, idp, arg);
			break;
		case DEVCMD_SET_TXIRQHDL:
			idp =  va_arg(arg_ptr, uartIdp);
			arg = va_arg(arg_ptr, void*);
			va_end(arg_ptr); 
			driUart->setIdp( driUart, DRCT_TX, idp, arg);
		
			break;
		
		
		case DEVCMD_GET_RXBUFPTR:
			ppdata =  va_arg(arg_ptr, void **);
			arg = va_arg(arg_ptr, void*);
			va_end(arg_ptr); 
			*( (int *)arg) = driUart->takeUpPlayloadBuf( driUart, ppdata);
			break;
		
		case DEVCMD_FREE_RXBUFPTR:
			arg = va_arg(arg_ptr, void*);
			va_end(arg_ptr); 
			driUart->giveBackPlayloadBuf( driUart, arg);
			break;
		case DEVCMD_SET_BAUD:
			int_data = va_arg(arg_ptr, int);
			va_end(arg_ptr); 
			DRI_Uart_Change_baud(driUart, int_data);
			break;
		default: break;
		
	}
	
	return RET_OK;
	
}

int Dev_Uart_test( I_dev_Char *self, void *testBuf, int len)
{
	Dev_Uart *cthis = ( Dev_Uart *)self;
	driveUart *driUart = cthis->dri;
	
	return driUart->test( driUart, testBuf, len);
}




static int UartWaitTxSem( void *self, int ms)
{
	Dev_Uart *cthis = ( Dev_Uart *)( ( ( driveUart*)self)->device);
	return Sem_wait(&cthis->txsem, ms);
	
	
//	int	ret;
//	Dev_Uart *cthis = ( Dev_Uart *)( ( ( driveUart*)self)->device);
//	while(1)
//	{
//		if(arr_tx_sem_val[cthis->minor])
//			break;
//		delay_ms(1);
//		if(ms)
//			ms --;
//		else
//			break;
//		
//	}
//	ret = arr_tx_sem_val[cthis->minor];
//	
//	if(arr_tx_sem_val[cthis->minor])
//		arr_tx_sem_val[cthis->minor] --;
//	return ret;

}
static int UartWaitRxSem( void *self, int ms)
{
//	int	ret;
	Dev_Uart *cthis = ( Dev_Uart *)( ( ( driveUart*)self)->device);
	return Sem_wait(&cthis->rxsem, ms);
//	while(1)
//	{
//		if(arr_rx_sem_val[cthis->minor])
//			break;
//		delay_ms(1);
//		if(ms)
//			ms --;
//		else
//			break;
//		
//	}
//	ret = arr_rx_sem_val[cthis->minor];
//	
//	if(arr_rx_sem_val[cthis->minor])
//		arr_rx_sem_val[cthis->minor] --;
//	return ret;
	
}

static void UartPostTxSem(void *self)
{
	Dev_Uart *cthis = ( Dev_Uart *)( ( ( driveUart*)self)->device);
	Sem_post(&cthis->txsem);
	
//	arr_tx_sem_val[cthis->minor] = 1;
	

}
static void UartPostRxSem( void *self)
{
	Dev_Uart *cthis = ( Dev_Uart *)( ( ( driveUart*)self)->device);
	Sem_post(&cthis->rxsem);
//	arr_rx_sem_val[cthis->minor] = 1;

}

static void UartLedRxHdl( void *self)
{
	
	
}

static void UartLedTxHdl( void *self)
{
	
	
}


CTOR( Dev_Uart)
FUNCTION_SETTING( I_dev_Char.open, Dev_Uart_open);
FUNCTION_SETTING( I_dev_Char.close, Dev_Uart_close);
FUNCTION_SETTING( I_dev_Char.read, Dev_Uart_read);
FUNCTION_SETTING( I_dev_Char.write, Dev_Uart_write);
FUNCTION_SETTING( I_dev_Char.ioctol, Dev_Uart_ioctol);
FUNCTION_SETTING( I_dev_Char.test, Dev_Uart_test);
END_CTOR

