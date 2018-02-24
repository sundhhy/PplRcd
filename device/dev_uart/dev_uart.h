#ifndef __DEV_UART1_
#define __DEV_UART1_
#include "basis/sdhError.h"
#include "lw_oopc.h"
#include "uart/drive_uart.h"


#include "dev_char.h"





CLASS( Dev_Uart)
{
	IMPLEMENTS( I_dev_Char);
	driveUart		*dri;
	int			txsem, rxsem;
//	void		*p_lock;
	void		*txled, *rxled;
	uint8_t		minor;
	uint8_t		none[3];
	
};



Dev_Uart *Get_DevUart(int minor);
#endif
